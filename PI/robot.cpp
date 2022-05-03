#include "robot.h"

robot::robot() {
    _dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    _video.open(0);
    robot::initPigpio();

    _currentState = STATE::FIND_T1; // inital state
    _targetID = ARUCOIDS::TARGET1; // inital target

    _canvas = cv::Mat::zeros(cv::Size(1,1), CV_8UC3); // initialize canvas
    cv::imshow("RGB", _canvas); // initialize image
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

    } while (_currentState == STATE::FIND_T1);
    std::cout << "STATE " << _currentState << " FINISHED" << std::endl;

    _currentState++;
}

void robot::videoFeed() {
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
                        }
                    }
                }
            }
            cv::line(_canvas, cv::Point(SCREEN_X/2, 0), cv::Point(SCREEN_X/2, SCREEN_Y), cv::Scalar(0,0,255));
            cv::line(_canvas, cv::Point(0, SCREEN_Y/2), cv::Point(SCREEN_X, SCREEN_Y/2), cv::Scalar(0,0,255));
            cv::imshow("RGB", _canvas);
        } while (thread_exit == false);
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
        } else if (_centre.x < SCREEN_X/2) {
            std::cout << "TURNING LEFT" << std::endl;
        }
    }
}

void robot::exitLoop() {
    while (cv::waitKey(10) != 'q');
    thread_exit = true;
}
