#pragma once
#include <string>
#include <thread>
#include <pigpio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include "CServo.h"
#include "server.h"

#define SCREEN_X 640
#define SCREEN_Y 480
#define IM_PORT 4699
#define TARGETTHRESH 10

//TX 8 (pi to teensy)
//RX 10 (teensy to pi)

enum ARUCOIDS {
    TARGET1 = 21,
    TARGET2 = 22,
    TARGET3 = 27,
    TARGET4 = 23
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
    FIND_T4
};

enum PINS {
     BUTTON = 23,
     LED = 21,
     TURRET = 20,
     LAUNCHER = 22
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
    cv::Mat _canvas; ///< canvas object
    cv::Ptr<cv::aruco::Dictionary> _dictionary; ///< dictionary for ArUco IDs
    cv::Point _centre; ///< stores the position of the current ARUCO centre of interest
    int _currentState = NULL_STATE; ///< int holding the current state the system is in
    int _targetID = NULL_STATE; ///< int holding the current target being looked for
    bool thread_exit = false; ///< condition to exit all threads
    bool _tracking = false; ///< whether or not the system is currently tracking
    CServo turretServo = CServo(PINS::TURRET); ///< servo object for turret
    CServo launcherServo = CServo(PINS::LAUNCHER); ///< servo object for launcher
    bool manual = false; ///< bool determining manual or automatic mode
    Server server; ///< Server object
    std::mutex serverMutex;
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
    void centreDefault() { _centre = cv::Point(SCREEN_X/2, SCREEN_Y/2); }

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
    bool getExit() {return this->thread_exit;}

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

};
