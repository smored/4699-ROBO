#include "robot.h"
#include "cvui.h"

robot::robot() {
    // Initialize variables
    _dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    centreDefault();
    _statemap[FIND_T1] = "ONE";
    _statemap[FIND_T2] = "TWO";
    _statemap[FIND_T3] = "THREE";
    _statemap[FIND_T4] = "FOUR";

    // Initialize states and targets
    _currentState = STATE::FIND_T1;
    _targetID = ARUCOIDS::TARGET1;

    // Initialize pigpio
    robot::initPigpio();
    gpioSetMode(PINS::LED, PI_OUTPUT);
    gpioSetMode(PINS::BUTTON, PI_INPUT);
    gpioSetPullUpDown(PINS::BUTTON, PI_PUD_UP); // pullup resistor on button
    _turretServo.resetServo();
    _launcherServo.resetServo();

    // Open Serial device
    initSerial();

    // Make settings window
    const cv::Size canvasSize = cv::Size(300*16/9, 300);
    _settings = cv::Mat::zeros(canvasSize, CV_8UC3);
    cv::imshow("SETTINGS", _settings);
    cvui::init("SETTINGS");
}

robot::~robot() {
    serClose(_serialHandle);
    _server.stop();
    _video.release();
    cv::destroyAllWindows();
    gpioTerminate();
}

void robot::runLoop() {
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

    // Start ui thread
    std::thread t6(&robot::uiElements, this);
    t6.detach();

    // Only care about states in auto mode
    if (!_manual) {
        do {
            // check if state is valid and set
            if (_currentState == STATE::NULL_STATE) {
                std::cerr << "[E]: State undefined" << std::endl;
                return;
            }

            //run through each state until it is finished
            auto lastState = _currentState;
            while(_currentState == lastState) {;}
            std::cout << "STATE " << lastState << " FINISHED" << std::endl;
            sendString(_statemap.at(lastState), 1000); // 1000 ms of timeout time
        } while (!_thread_exit);
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
        cv::Mat canvas; // temporary canvas buffer for flipping image
        do {
            _video >> canvas;
            cv::flip(canvas, _canvas, -1);
            //_canvas = canvas;
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
                            _centre = centre;
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

            cv::line(_canvas, cv::Point(SCREEN_X*(50+_targetThresh)/100, 0), cv::Point(SCREEN_X*(50+_targetThresh)/100, SCREEN_Y), cv::Scalar(0,255,0));
            cv::line(_canvas, cv::Point(SCREEN_X*(50-_targetThresh)/100, 0), cv::Point(SCREEN_X*(50-_targetThresh)/100, SCREEN_Y), cv::Scalar(0,255,0));

            cv::imshow("RGB", _canvas);
            if (_thread_exit) break;
        } while (cv::waitKey(10) != 'q');
    } else {
        std::cerr << "Video Feed not defined!" << std::endl;
    }
    _thread_exit = true;
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

void robot::initSerial() {
    std::cout << "Initializing Serial..." << std::endl;
    _serialHandle = serOpen(UART_ADDR, BAUD_RATE, 0);
    if (_serialHandle > 0) {
        std::cout << "Serial Successfully Initialized" << std::endl;
    } else {
        std::cout << "Failure to Initialize Serial" << std::endl;
    }
}

void robot::aimCannon() {
    while (!_thread_exit) {
        if (!_tracking) continue;

        try {

        static bool firing = false;
        const auto awaketime = std::chrono::system_clock::now() + std::chrono::milliseconds(_turretServo.getDelay());
        const auto thresh_L = 50 - _targetThresh, thresh_R = 50 + _targetThresh;
        int centrey = _centre.x;
        double percentageY = ((double) centrey/(double)SCREEN_X)*100;
        std::cout << "CENTRE Y: " << centrey << std::endl;
        std::cout << "PERCENTAGE Y: " << percentageY << std::endl;

        // if within thresholds, fire, otherwise turn to match
        if (percentageY < thresh_R && percentageY > thresh_L) {
            if (!firing) { // make sure two threads can never fire simultaneously
                firing = true;
                std::cout << "FIRING!" << std::endl;
                fireCannon();
                firing = false;
            }
        } else {
            if (percentageY > thresh_R) {
                std::cout << "TURNING RIGHT" << std::endl;
                _turretServo.add(false);
            } else if (percentageY < thresh_L) {
                std::cout << "TURNING LEFT" << std::endl;
                _turretServo.add(true);
            }
        }

        _turretServo.moveServo();
        std::this_thread::sleep_until(awaketime);
        } catch (std::exception e) {
            std::cerr << "aimCannon() exception at: " << e.what() << std::endl;
        }
    }
}

void robot::fireCannon() {
    _launcherServo.setPos(800);
    _launcherServo.moveServo();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    _launcherServo.setPos(1500);
    _launcherServo.moveServo();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void robot::serverReceive() {
	std::vector<std::string> cmds;

	do {
		_server.get_cmd(cmds);
		if (cmds.size() > 0) {
			for (int i = 0; i < cmds.size(); i++) {
                std::string command = cmds.at(i);
                if (command == "im") continue; // ignore im here; is handled by start()
                // do something with the command
                std::cout << "server command at index: " << std::to_string(i) << " : " << cmds.at(i) << std::endl;
                std::string reply = cmds.at(i) + " Received";
                _server.send_string(reply);
            }
		}
	} while (!_thread_exit);
}


void robot::serverSendIm() {
    do {
        // process message??
        _server.set_txim(_canvas);
    }
    while (!_thread_exit);
}

void robot::serverThread() {
	_server.start(IM_PORT);
}

void robot::uiElements() {
    const cv::Size canvasSize = cv::Size(300*16/9, 300);
    double turretDelay = 50, turretSpeed = 10;
    do {
        _settings = cv::Mat::zeros(_settings.size(), CV_8UC3);

        cvui::text(_settings, 20, 20, "Threshold Size");
        cvui::trackbar(_settings, 0, 40, 200, &_targetThresh, (double) 0, (double) 50);

        cvui::text(_settings, 20, 100, "Turret Delay");
        cvui::trackbar(_settings, 0, 120, 200, &turretDelay, (double) 0, (double) 200);
        _turretServo.setDelay(turretDelay);

        cvui::text(_settings, 20, 180, "Turret Speed");
        cvui::trackbar(_settings, 0, 200, 200, &turretSpeed, (double) 0, (double) 200);
        _turretServo.setDelay(turretDelay);

        if (cvui::button(_settings, 20, 260, "Manually Increment State")) {
            _currentState++;
        }

        cv::imshow("SETTINGS", _settings);
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // poll this slower so that other threads can run faster
    } while(!_thread_exit);
}

void robot::sendString(std::string input, unsigned int timeoutTime) {
    auto timeout =  std::chrono::system_clock::now() + std::chrono::milliseconds(timeoutTime);
    std::cout << "Attempting to send: " << input << std::endl;
    do {
        // check if sent ok. serWrite() returns zero if success
        if (serWrite(_serialHandle, &input[0], input.length())) { // this is dumb but it wants a char*. I hope i did it right
            std::cout << "Failed to send string over UART; serWrite() returned bad value!" << std::endl;
            continue;
        }
        // then wait for a response to affirm a successfully sent message
        if (serDataAvailable(_serialHandle) >= 0) {
            std::cout << "Sent message!" << std::endl;
            return;
        }
    } while (std::chrono::system_clock::now() < timeout);

    // timed out
    std::cout << "Failed to send string over UART; Timed out!" << std::endl;
    return;
}
