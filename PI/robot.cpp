#include "robot.h"

robot::robot() {
    // Initialize variables
    _dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

    // Initialize states and targets
    _currentState = STATE::FIND_T1;
    _targetID = ARUCOIDS::TARGET1;

    // Initialize Canvas
    //_canvas = cv::Mat::zeros(cv::Size(600,300), CV_8UC3);
    //cv::imshow("RGB", _canvas);

    // Initialize pigpio
    robot::initPigpio();
    gpioServo(PINS::SERVO, SERVOENUM::POS_MIDDLE);
    gpioSetMode(PINS::LED, PI_OUTPUT);
    gpioSetMode(PINS::BUTTON, PI_INPUT);
    gpioSetPullUpDown(PINS::BUTTON, PI_PUD_UP); // pullup resistor on button
}

robot::~robot() {
    _video.release();
    cv::destroyAllWindows();
    gpioTerminate();
}

void robot::runLoop() {
    std::thread t1(&robot::videoFeed, this); // run videoFeed on its own thread, will tell us the IDs it sees and will locate the centre of the ID of interest
    t1.detach();
    //std::thread t2(&robot::exitLoop, this);
    //t2.detach();

    // check if state is valid and set
    if (_currentState == STATE::NULL_STATE) {
        std::cerr << "[E]: State undefined" << std::endl;
        return;
    }

    //run through each state until it is finished
    do {
        if (gpioRead(PINS::BUTTON)) thread_exit = true;
        if (thread_exit == true) break;
        static auto localpos = _centre;
        if (localpos != _centre) aimCannon();
        localpos = _centre;
    } while (_currentState == STATE::FIND_T1);
    std::cout << "STATE " << _currentState << " FINISHED" << std::endl;

    _currentState++;
}

void robot::videoFeed() {
    std::cout << "attempting video open\n";
    _video.open(0);
    if (_video.isOpened()) std::cout << "video opened\n";
    if (_video.isOpened()) {
        do {
            _video >> _canvas;
            if (!_canvas.empty()) {
                std::vector<int> ids;
                std::vector<std::vector<cv::Point2f>> corners;
                cv::aruco::detectMarkers(_canvas, _dictionary, corners, ids);
                if (ids.size() > 0) {
                    cv::aruco::drawDetectedMarkers(_canvas, corners, ids);

                    for (int i = 0; i < ids.size(); i++) { // Find centre using rectangle of two points
                        cv::Rect centresquare = cv::Rect(corners.at(i).at(0), corners.at(i).at(2));
                        cv::Point centre = cv::Point(centresquare.x + centresquare.width/2, centresquare.y + centresquare.height/2);
                        cv::circle(_canvas, centre, 3, (0,0,255));

                        // store location of centre of ARUCO of interest
                        if (ids.at(i) == _targetID) {
                            _centre = centre;
                            std::cout << "centre of aruco: " << _centre << " With ID: " << ids.at(i) << std::endl;
                        }
                    }
                }
            } else {
                std::cerr << "Cavnas Empty!" << std::endl;
            }
            cv::line(_canvas, cv::Point(SCREEN_X/2, 0), cv::Point(SCREEN_X/2, SCREEN_Y), cv::Scalar(0,0,255));
            cv::line(_canvas, cv::Point(0, SCREEN_Y/2), cv::Point(SCREEN_X, SCREEN_Y/2), cv::Scalar(0,0,255));
            cv::imshow("RGB", _canvas);
        } while (thread_exit == false);
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

bool robot::aimCannon() {
    // calculates angle through pwm duty cycle
    /*// save some processing power by ignoring the command if its the same as the previous
    static int prevInput;
    if (prevInput != _setPos) {
        int microseconds = (_setPos/180) * 1000 + 1000;
        gpioServo(SERVO_CHANNEL, microseconds);
    }
    prevInput = _setPos;*/

    if (SCREEN_X/2 + 20 < _centre.x && SCREEN_X/2 - 20 > _centre.x) {
        std::cout << "FIRING!" << std::endl;
    } else {
        if (_centre.x > SCREEN_X/2) {
            std::cout << "TURNING RIGHT" << std::endl;
            gpioServo(PINS::SERVO, SERVOENUM::POS_RIGHT);
        } else if (_centre.x < SCREEN_X/2) {
            std::cout << "TURNING LEFT" << std::endl;
            gpioServo(PINS::SERVO, SERVOENUM::POS_LEFT);
        }
    }
}

void robot::exitLoop() {
    while (cv::waitKey(10) != 'q');
    thread_exit = true;
}