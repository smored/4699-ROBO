#pragma once
#include <string>
#include <pigpio.h>
#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>

#define MAX_VAL (float)((2^10)-1)
#define DEBOUNCE_TIME 0
#define TIMEOUT 100

enum { DIGITAL, ANALOG, SERVO };

/** CControlPi.h
 *
 * @brief Methods for communcation using SPI protocol
 *
 * This class is a revamped version of the old CControl for use with the raspberry pi SPI port.
 * It has the same functionality as the old version.
 *
 * @author Kurt Querengesser
 *
 * @version 1.0 -- 03 March 2022
 */
class CControlPi {
public:

	/** @brief Class Constructor
	* @return void
	**/
	CControlPi();

	/** @brief Class Deconstructor
	* @return void
	**/
	~CControlPi();

	/** @brief reads data from pi GPIO
	* @param type: what mode to operate
	* @param channel: which channel to use
	* @param result: a pointer to the result to which the value will be assigned
	* @return returns true if successful
	**/
	bool get_data(int type, int channel, int& result);

	/** @brief helper function for reading the ADC's values using SPI
	* @return int
	**/
	int readSPI(int channel);

	/** @brief sets pi GPIO pin states
	* @param type: what mode to operate
	* @param channel: which channel to use
	* @param val: what value to set
	* @return returns true if successful
	**/
	bool set_data(int type, int channel, int val);

	/** @brief debounces a button on specified channel (active low)
	* @param channel: button GPIO pin
	* @param BID: specify channel button is on
	* @return returns debounced digital button boolean
	**/
	bool get_button(int channel);

	/** @brief Gets analog over serial
	* @param channel: what pin to use
	* @return returns an analog float value as a percentage of maximum
	**/
	float get_analog(int channel);

	/** @brief Method for setting servo
	* @param channel: Select which servo channel to use
	* @param val: the angle to which the servo will approach
	* @return void
	**/
	void set_servo(int setPos, int channel);
};
