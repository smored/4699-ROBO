#include "robot.h"

robot::robot() {
    // Initialize variables
    _dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    centreDefault();

    // Initialize states and targets
    _currentState = STATE::FIND_T1;
    _targetID = ARUCOIDS::TARGET1;

    // Initialize pigpio
    robot::initPigpio();
    gpioSetMode(PINS::LED, PI_OUTPUT);
    gpioSetMode(PINS::BUTTON, PI_INPUT);
    gpioSetPullUpDown(PINS::BUTTON, PI_PUD_UP); // pullup resistor on button

    // Start robot threads
    std::thread t1(&robot::videoFeed, this);
    t1.detach();
    std::thread t2(&robot::aimCannon, this);
    t2.detach();

    // Start server threads
    std::thread t3(&robot::serverReceive, this); // receive commands from client
    t3.detach();
    std::thread t4(&robot::serverSendIm, this); // send images repeatedly to client
    t4.detach();
    std::thread t5(&robot::serverThread, this); // run the server thread
    t5.detach();
}

robot::~robot() {
    server.stop();
    _video.release();
    cv::destroyAllWindows();
    gpioTerminate();
}

void robot::runLoop() {

    // Only care about states in auto mode
    if (!manual) {
        // check if state is valid and set
        if (_currentState == STATE::NULL_STATE) {
            std::cerr << "[E]: State undefined" << std::endl;
            return;
        }
        //run through each state until it is finished
        do {
            if (!gpioRead(PINS::BUTTON)) thread_exit = true;
            if (thread_exit == true) break;

            // update pid loop
          //  PIDController::tick();
        } while (1);
        std::cout << "STATE " << _currentState << " FINISHED" << std::endl;

        _currentState++;
    }
}

void robot::videoFeed() {
    std::cout << "attempting video open\n";
    try {
        _video.open(0);
    } catch (std::exception e) {
        std::cerr << "videoFeed() exception at: " << e.what() << std::endl;
    }
    if (_video.isOpened()) {
        std::cout << "video opened\n";
        do {
            _video >> _canvas;
            if (!_canvas.empty()) {
                std::vector<int> ids;
                std::vector<std::vector<cv::Point2f>> corners;
                cv::aruco::detectMarkers(_canvas, _dictionary, corners, ids);
                if (ids.size() > 0) {
                    _tracking = true;
                    cv::aruco::drawDetectedMarkers(_canvas, corners, ids);

                    for (int i = 0; i < ids.size(); i++) { // Find centre using rectangle of two points
                        cv::Rect centresquare = cv::Rect(corners.at(i).at(0), corners.at(i).at(2));
                        cv::Point centre = cv::Point(centresquare.x + centresquare.width/2, centresquare.y + centresquare.height/2);
                        //cv::circle(_canvas, centre, 3, (0,0,255));

                        // store location of centre of ARUCO of interest
                        if (ids.at(i) == _targetID) {
                            serverMutex.lock();
                            _centre = centre;
                            serverMutex.unlock();
                            //std::cout << "centre of aruco: " << _centre << " With ID: " << ids.at(i) << std::endl;
                        }
                    }
                } else {
                    // if no IDs are found, turn off tracking, so servo doesnt run off
                    _tracking = false;
                }
            } else {
                std::cerr << "Cavnas Empty!" << std::endl;
            }
            cv::line(_canvas, cv::Point(SCREEN_X/2, 0), cv::Point(SCREEN_X/2, SCREEN_Y), cv::Scalar(0,0,255));
            cv::line(_canvas, cv::Point(0, SCREEN_Y/2), cv::Point(SCREEN_X, SCREEN_Y/2), cv::Scalar(0,0,255));
            cv::imshow("RGB", _canvas);
        } while (cv::waitKey(10) != 'q');
    } else {
        std::cerr << "Video Feed not defined!" << std::endl;
        thread_exit = true;
    }
}

void robot::initPigpio() {
    std::cout << "Initializing GPIO..." << std::endl;
    if (gpioInitialise() >= 0) {
        std::cout << "GPIO Successfully Initialized" << std::endl;
    } else {
        std::cout << "Failure to Initialize GPIO" << std::endl;
        gpioTerminate();
    }
}

void robot::aimCannon() {
    while (!thread_exit) {
        if (!_tracking) continue;

        try {

        const auto awaketime = std::chrono::system_clock::now() + std::chrono::milliseconds(cservo.getDelay());
        static const auto thresh_L = 40, thresh_R = 60;
        serverMutex.lock();
        int centrey = _centre.y;
        serverMutex.unlock();
        double percentageY = ((double) centrey/(double)SCREEN_Y)*100;
        std::cout << "CENTRE Y: " << centrey << std::endl;
        std::cout << "PERCENTAGE Y: " << percentageY << std::endl;

        // if within thresholds, fire, otherwise turn to match
        if (percentageY < thresh_R && percentageY > thresh_L) {
            std::cout << "FIRING!" << std::endl;
        } else {
            if (percentageY > thresh_R) {
                std::cout << "TURNING LEFT" << std::endl;
                cservo.add(true);
            } else if (percentageY < thresh_L) {
                std::cout << "TURNING RIGHT" << std::endl;
                cservo.add(false);
            }
        }

        cservo.moveServo();
        //std::cout << "ServoPos: " << servoPos << std::endl;
        std::this_thread::sleep_until(awaketime);
        } catch (std::exception e) {
            std::cerr << "aimCannon() exception at: " << e.what() << std::endl;
        }
    }
}


void robot::serverReceive() {
	std::vector<std::string> cmds;

	do {
		server.get_cmd(cmds);
		if (cmds.size() > 0) {
			for (int i = 0; i < cmds.size(); i++) {
                std::string command = cmds.at(i);
                if (command == "im") continue; // ignore im commands here
                // do something with the command
                std::cout << "server command at index: " << std::to_string(i) << " : " << cmds.at(i) << std::endl;
                std::string reply = cmds.at(i) + " Received";
                server.send_string(reply);
            }
		}
	} while (!thread_exit);
}

void robot::serverSendIm() {
	if (_video.isOpened()) {
		do {
			if (!_canvas.empty()) {
				// process message??
				serverMutex.lock();
				server.set_txim(_canvas);
				serverMutex.unlock();
			}
		}
		while (!thread_exit);
	}
}

void robot::serverThread() {
	server.start(IM_PORT);
}
