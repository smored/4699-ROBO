////////////////////////////////////////////////////////////////
// ELEX 4618 Client Template project for BCIT
// Created Oct 5, 2016 by Craig Hennessey
// Last updated March 29, 2019
////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <string>
#include <iostream>
#include <thread>
#include <regex>
#include "Client.h"

Client client(4618, "192.168.102.175");
std::mutex txLock;

void process_msg()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

void print_menu()
{
	std::cout << "\n***********************************";
	std::cout << "\n* ELEX4618 Client Project";
	std::cout << "\n***********************************";
	//std::cout << "\n(1) Get image";
	std::cout << "\n(1) Send A";
	std::cout << "\n(2) Toggle System MANUAL/AUTOMATIC";
	std::cout << "\n(3) Sort Ball Left";
	std::cout << "\n(4) Sort Ball Right";
	std::cout << "\n(0) Exit";
	std::cout << "\nCMD> ";
}

/* ------ - NYI--------
std::string get_rx() {
	bool rx_success = false;
	std::string rx_str = "";

	do {
		client.tx_str("req");
		client.rx_str(rx_str);
		if (!std::regex_search(rx_str, std::regex(",")) || !std::regex_search(rx_str, std::regex(";"))) {
			std::cout << "\nInvalid String... Trying Again\n";
			continue;
		}
		else {
			std::cout << "\nSuccessful String Match\n";
			return rx_str;
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
	} while (!rx_success);
}*/

void count_ball(std::string input) {
	int delimit1 = -1, delimit2 = -1, delimit3 = -1;
	int i = 0;
	try {
		for (char ch : input) { // Find location of delimiters
			if (ch == ';') {
				delimit1 = i;
			}
			else if (ch == ',') {
				delimit2 = i;
			}
			else if (ch == '\n') {
				delimit3 = i;
			}
			i++;
		}
	}
	catch (std::exception e) {
		std::cerr << e.what() << " in for each" << std::endl;
	}

	if (delimit1 == -1 || delimit2 == -1 || delimit3 == -1) {
		std::cerr << "Error, Invalid String\n";
		return;
	}

	// Read numbers based on delimiters
	try {
		std::string num1 = input.substr(delimit1+1, delimit2-1 - delimit1);
		std::cout << "Ball Left Count: " << num1 << std::endl;
		std::string num2 = input.substr(delimit2+1, delimit3 - delimit2);
		std::cout << "Ball Right Count: " << num2 << std::endl;
	}
	catch (std::exception e) {
		std::cerr << e.what() << " in substring" << std::endl;
	}
}

void send_command(std::string cmd)
{
	std::string str;

	txLock.lock();
	client.tx_str(cmd);
	txLock.unlock();

	do
	{
		txLock.lock();
		client.rx_str(str);
		txLock.unlock();
		if (str.length() > 0)
		{
			std::cout << "\nClient Rx: " << str;
			count_ball(str);
		}
	} while (str.length() == 0);
}


void send_a()// deprecated
{
	std::string str;

	txLock.lock();
	client.tx_str("a");
	txLock.unlock();

	do
	{
		client.rx_str(str);
		if (str.length() > 0)
		{
			std::cout << "\nClient Rx: " << str;
		}
	} while (str.length() == 0);
}

void get_image()
{
	cv::Mat im;
	bool rx_success = true;

  do {
	 
	  if (rx_success) {
		  txLock.lock();
		  client.tx_str("im");
		  txLock.unlock();
	  }

	  txLock.lock();
	  if (client.rx_im(im)) {
		  rx_success = true;
	  }
	  else {
		  rx_success = false;
	  }
	  txLock.unlock();

	  if (rx_success && !im.empty()) {
		  cv::imshow("rx", im);
	  }

	  /*
	  if (client.rx_im(im)) { 
		  rx_success = true;
		  if (im.empty() == false)
		  {
			  cv::imshow("rx", im);
		  }
	  }
	  else {
		  rx_success = false;
	  }*/

  } while (cv::waitKey(100) != 'q');
}


int main(int argc, char* argv[])
{
	std::thread t1(&get_image);
	t1.detach();

	int cmd = -1;
	do
	{
		print_menu();
		std::cin >> cmd;
		switch (cmd)
		{
		//case 1: send_command(); break;
		//case 1: get_image(); break;
		case 1: send_a(); break;
		case 2: send_command("toggle"); break;
		case 3: send_command("left"); break;
		case 4: send_command("right"); break;
		default: continue;
		}
	} while (cmd != 0);

}
