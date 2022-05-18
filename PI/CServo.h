#pragma once
#include <pigpio.h>
#include <iostream>
#include <thread>

#define DEFAULT_SPEED 10
#define DEFAULT_DELAY 50
//#define DEFAULT_POS 2500

/** CServo.h
 *
 * @brief Servo Class for holding data
 *
 * @author Kurt Querengesser
 *
 * @version 1.0 -- 09 May 2022
 */
class CServo {
private:
    int _gpioPin;
    int _pos;
    int _speed;
    int _delay;
    const int DEFAULT_POS;
    //int _lastPos;
public:

    // Default constructor
    CServo(int gpio, int speed = DEFAULT_SPEED, int delay = DEFAULT_DELAY, int startpos = 2500) : DEFAULT_POS(startpos) {
        _gpioPin = gpio;
        _pos = startpos;
       // _lastPos = _pos;
        _speed = speed;
        _delay = delay;
      }

    ~CServo() {;}

    const int middle = 1500; ///< middle position of servo
    const int left = 2500; ///< left position of servo
    const int right = 500; ///< right position of servo

    /**
    * @brief setter for speed
    */
    void setSpeed(int speed) { this->_speed = speed; }

    /**
    * @brief setter for delay
    */
    void setDelay(int delay) { if (delay > 0) this->_delay = delay; }

    /**
    * @brief getter for speed
    */
    int getSpeed() { return this->_speed; }

    /**
    * @brief getter for delay
    */
    int getDelay() { return this->_delay; }

    /**
    * @brief Method used for repositioning the servo. adds or subtracts the speed from the desired position Does not move the servo head. Call moveServo() to update head position
    * @param positive: whether to add or subtract speed value
    */
    void add(bool positive) {
        int pos = _pos;
        if (positive) pos += _speed;
        if (!positive) pos -= _speed;
        if (pos > left) pos = left;
        if (pos < right) pos = right;
        this->_pos = pos;
    }

    /**
    * @brief Method used for setting the servo position absolutely. Call moveServo() to update head position
    * @param pos: position to move to in milliseconds (500 - 2500)
    */
    void setPos(int pos) {
        if (pos > left) pos = left;
        if (pos < right) pos = right;
        this->_pos = pos;
    }

    /**
    * @brief Updates the position of the servo head
    */
    void moveServo() {
        gpioServo(_gpioPin, _pos);
    }


    /**
    * @brief returns the servo to the default position
    */
    void resetServo() {_pos = DEFAULT_POS; moveServo(); std::this_thread::sleep_for(std::chrono::milliseconds(500)); }
};
