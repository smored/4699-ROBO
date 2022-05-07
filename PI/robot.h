#pragma once
#include <string>
#include <thread>
#include <pigpio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include "PID.h"

#define SCREEN_X 640
#define SCREEN_Y 360

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
 SERVO = 20,
 TX = 8,
 RX = 10
};

enum SERVOENUM {
 POS_MIDDLE = 1500,
 POS_LEFT = 2500,
 POS_RIGHT = 500,
 // how much to move the servo by every tick
 SPEED =  10,
 // how long each tick should take in ms
 DELAY = 50

 // mapping scale factor of 7.4x + 500
 // i.e 90 degrees is 90*7.4 + 500
 // = 1166
 //NORTH = 1200,
 //EAST = 500,
 //WEST = 1800
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
    int _setPos = 90; ///< servo's destination position
    bool thread_exit = false; ///< condition to exit all threads
    bool _tracking = false; ///< whether or not the system is currently tracking
    PIDController controller; ///< Main component of PID
public:

    // default constructor
    robot();

    // default destructor
    ~robot();

     // main run function
    void runLoop();

    // loops camera for video feed
    void videoFeed();

    // starts up the PIGPIO library
    void initPigpio();

    // tells servo to go to a certain position
    void setServo();

    // transmits out data to the secondary processor for driving
    void txData();

    // aims servo at ARUCO
    void aimCannon();

    // runs a loop checking for when to exit program
    void exitLoop();

    // sets centre member to default
    void centreDefault() {_centre = cv::Point(SCREEN_X/2, SCREEN_Y/2);}


};
