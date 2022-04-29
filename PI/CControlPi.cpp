#include "CControlPi.h"

CControlPi::CControlPi() {
    std::cout << "Initializing GPIO..." << std::endl;
	if (gpioInitialise() >= 0) {
        std::cout << "GPIO Successfully Initialized" << std::endl;
	} else {
	    std::cout << "Failure to Initialize GPIO" << std::endl;
	}
}

CControlPi::~CControlPi() {
    std::cout << "Terminating GPIO..." << std::endl;
    gpioTerminate();
}

bool CControlPi::get_data(int type, int channel, int& result) {
    // dont forget to  gpioSetMode() in main()
    switch (type) {
    case DIGITAL:
        result = gpioRead(channel);
        return true;
    case ANALOG:
        result = readSPI(channel);
        return true;
    default:
    return false;
    }
}

int CControlPi::readSPI(int channel) {
    int read_val;
    unsigned char inBuf[3];
	char cmd[] = {1, 128|(channel<<4), 0}; // 0b1XXX0000 where XXX=channel 0
	int handle = spiOpen(0, 200000, 3); // Mode 0, 200kHz
	spiXfer(handle, cmd, (char*) inBuf, 3); // Transfer 3 bytes
	read_val = ((inBuf[1] & 3) << 8) | inBuf[2]; // Format 10 bits
	spiClose(handle); // Close SPI system
	return read_val;
}

bool CControlPi::set_data(int type, int channel, int val) {
    switch (type) {
    case DIGITAL:
        gpioWrite(channel, val);
        return true;
    case ANALOG: // bad implementatin of analog, only supports 5%-10% Duty Cycle
    case SERVO:
        gpioServo(channel, val);
        return true;
    default:
    return false;
    }
}

bool CControlPi::get_button(int channel) {

	static bool reset[] = {0,0,0};
	static auto start = std::chrono::_V2::system_clock::now();
	int result;
	bool isPressed;

	get_data(DIGITAL, channel, result); isPressed = result == 0;

	if (std::chrono::_V2::system_clock::now() - start > std::chrono::milliseconds(250)) {
        if (isPressed) {
        start = std::chrono::_V2::system_clock::now();
            return true;
        }
	}

	return false;

    /*
	if (!reset[BID]) {
		if (isPressed) {
			reset[BID] = true;
			return true;
		}
	}
	else {
		if (!isPressed) {
			reset[BID] = false;
		}
	}

	return false;
	*/
}

// Return as a percentage of full range
float CControlPi::get_analog(int channel) {
	int result;
	get_data(ANALOG, channel, result);
	return (float)result/(MAX_VAL);
}

void CControlPi::set_servo(int setPos, int channel) {
    // 1ms is leftmost, 2ms is rightmost. method takes in as microseconds
	int microseconds = (setPos/180) * 1000 + 1000;
    gpioServo(channel, microseconds);
}
