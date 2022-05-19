#pragma once
#include <string>
#include <thread>
#include <pigpio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include "CServo.h"
#include "server.h"
#include <unordered_map>

#define SCREEN_X 640
#define SCREEN_Y 480
#define IM_PORT 54420
#define UART_ADDR "/dev/serial0"
#define BAUD_RATE 9600

#define MAX_SHOTS 1

//TX 8 (pi to teensy)
//RX 10 (teensy to pi)

enum ARUCOIDS {
    TARGET1 = 21,
    TARGET2 = 22,
    TARGET3 = 27,
    TARGET4 = 23,
    TARGET5 = 29
    //WALL_N
    //WALL_E
    //WALL_S
    //WALL_W
};

enum STATE {
    NULL_STATE,
    FIND_T1,
    FIND_T2,
    FIND_T3,
    FIND_T4,
    DONE
};

enum PINS {
     BUTTON = 23,
     LED = 21,
     TURRET = 17,
     LAUNCHER = 22,

     REV = 6,
     RGT = 13,
     LFT = 19,
     FWD = 26
};

enum MAN_CMDS {
    SHOOT,
    FORWARD,
    LEFT,
    RIGHT,
    BACK
};


/** robot.h
 *
 * @brief header for robot main functions
 *
 * @author Kurt Querengesser
 *
 * @version 1.0 -- 02 May 2022
 */
class robot {
private:
    cv::VideoCapture _video; ///< video capture object
    cv::Mat _canvas, _settings; ///< canvas objects
    cv::Ptr<cv::aruco::Dictionary> _dictionary; ///< dictionary for ArUco IDs
    cv::Point _centre; ///< stores the position of the current ARUCO centre of interest
    int _currentState = NULL_STATE; ///< int holding the current state the system is in
    int _targetID = NULL_STATE; ///< int holding the current target being looked for
    bool _thread_exit = false; ///< condition to exit all threads
    bool _tracking = false; ///< whether or not the system is currently tracking
    CServo _turretServo = CServo(PINS::TURRET, 6, 50, 1800); ///< servo object for turret
    CServo _launcherServo = CServo(PINS::LAUNCHER); ///< servo object for launcher
    const bool MANUAL = true; ///< bool determining manual or automatic mode
    Server _server; ///< Server object
    double _targetThresh = 10;
    int _serialHandle{};
    std::unordered_map<int, std::string> _statemap;
    std::unordered_map<int, int> _targetmap;
    int _shots; ///< count how many shots to take per target
public:

    /** @brief default constructor
    */
    robot();

    /** @brief default destructor
    */
    ~robot();

    /** @brief main run function
    */
    void runLoop();

    /** @brief Method for determining ARUCO IDs it sees and will locate the centre of the ID of interest
    */
    void videoFeed();

    /** @brief starts up the PIGPIO library
    */
    void initPigpio();

    /** @brief starts up the Serial function from PIGPIO
    */
    void initSerial();

    /** @brief aims servo at ARUCO
    */
    void aimCannon();

    /** @brief fires the cannon
    */
    void fireCannon();

    /** @brief runs a loop checking for when to exit program
    */
    void exitLoop();

    /** @brief sets centre member to default
    */
    void centreDefault() { _centre = cv::Point(0, 0); }

    // getter for milliseconds as int
    //unsigned long int getMillisAsInt() { unsigned long int millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); return millis; }

    /** @brief gets centre of ARUCO
    */
    int getCentre() {return this->_centre.y;}

    /** @brief sets the output calculated by the PID object
    */
    //void setOut(int out) {cservo.setSpeed(out);}

    /** @brief gets thread exit status
    */
    bool getExit() {return this->_thread_exit;}

    /** @brief gets tracking status
    */
    bool getTracking() {return this->_tracking;}

    /** @brief method to be run by a thread for receieving commands
    */
    void serverReceive();

    /** @brief method to be run by a thread for sending images
    */
    void serverSendIm();

    /** @brief method to be run by a thread for running the server
    */
    void serverThread();

    /** @brief loop for running cvui elements
    */
    void uiElements();

    /** @brief method for attempting sending a string accross UART
    * @param input: string to send accross uart
    * @param timout: time in milliseconds for how long to try before timing out
    */
    void sendString(std::string input);

    void sendChar(char input);
};
