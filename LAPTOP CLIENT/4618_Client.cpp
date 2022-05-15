////////////////////////////////////////////////////////////////
// ELEX 4618 Client Template project for BCIT
// Created Oct 5, 2016 by Craig Hennessey
// Last updated April 2022
////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <string>
#include <iostream>
#include <thread>

#include "Client.h"

CClient craigClient;
CClient piClient;

void print_menu()
{
	std::cout << "\n***********************************";
	std::cout << "\n* ELEX4618 Client Project";
	std::cout << "\n***********************************";
	std::cout << "\n(1) Get current state";
	std::cout << "\n(2) Send get image command";
	std::cout << "\n(0) Exit";
	std::cout << "\nCMD> ";
}

/*
void send_command(CClient &client, std::string cmd)
{
	std::string str;

	client.tx_str(cmd);
	std::cout << "\nClient Tx: " << cmd;

	if (cmd == "G 1")
	{
		cv::Mat im;
		if (client.rx_im(im) == true)
		{
			if (im.empty() == false)
			{
				std::cout << "\nClient Rx: Image received";
				cv::imshow("rx", im);
				cv::waitKey(10);
			}
		}
		else
		{
			// No response, disconnect and reconnect
			client.close_socket();
			client.connect_socket(server_ip, server_port);
		}
	}
	else
	{
		if (client.rx_str(str) == true)
		{
			std::cout << "\nClient Rx: " << str;
		}
		else
		{
			// No response, disconnect and reconnect
			client.close_socket();
			client.connect_socket(server_ip, server_port);
		}
	}
}
*/

void getTopDown() {
	cv::Mat im;

	do {

		craigClient.tx_str("G 1");

		
		if (craigClient.rx_im(im)) {
			if (!im.empty()) {
				cv::imshow("rx", im);
				std::cout << "im received\n";
			}
			else {
				std::cout << "blank image received\n";
			}
		}
		else {
			std::cout << "no image received\n";
		}

	} while (cv::waitKey(100) != 'q');
}

void getPiFeed() {
	cv::Mat im;

	do {

		piClient.tx_str("im");


		if (piClient.rx_im(im)) {
			if (!im.empty()) {
				cv::imshow("rx2", im);
				std::cout << "im received\n";
			}
			else {
				std::cout << "blank image received\n";
			}
		}
		else {
			std::cout << "no image received\n";
		}

	} while (cv::waitKey(100) != 'q');
}

int main(int argc, char* argv[])
{
	craigClient.connect_socket("192.168.1.100", 46991);
	std::thread t1(&getTopDown);
	t1.detach();

	piClient.connect_socket("192.168.1.3", 69420);
	std::thread t2(&getPiFeed);
	t2.detach();

	while (1);
}
