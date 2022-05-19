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

enum MAN_CMDS {SHOOT, FORWARD, LEFT, RIGHT, BACK};

MAN_CMDS inputCmds;

void send_command(CClient& client, std::string cmd);

void print_menu()
{
	std::cout << "\n***********************************";
	std::cout << "\n* ELEX4618 Client Project";
	std::cout << "\n***********************************";
	std::cout << "\n(1) Get current state";
	std::cout << "\n(2) Manual Controlling";
	std::cout << "\n(3) TEST";
	std::cout << "\n(X) Exit";
	std::cout << "\nCMD> ";
}

void test_menu()
{
	int cmd = -1;
	do
	{
		std::cout << "\n***********************************";
		std::cout << "\n* TEST Menu";
		std::cout << "\n***********************************";
		std::cout << "\n(7) AUTO";
		std::cout << "\n(1) START";
		std::cout << "\n(2) ONE";
		std::cout << "\n(3) TWO";
		std::cout << "\n(4) THREE";
		std::cout << "\n(5) FOUR";
		std::cout << "\n(0) STOP";
		std::cout << "\n(X) Exit";
		std::cout << "\nCMD> ";
		std::cin >> cmd;
		switch (cmd)
		{
			case 7: send_command(piClient, "7"); break;
			case 0: send_command(piClient, "0"); break;
			case 1: send_command(piClient, "1"); break;
			case 2: send_command(piClient, "2"); break;
			case 3: send_command(piClient, "3"); break;
			case 4: send_command(piClient, "4"); break;
			case 5: send_command(piClient, "5"); break;
		}
	} while (cmd != 'X');
}


void send_command(CClient &client, std::string cmd)
{
	std::string str;

	client.tx_str(cmd);
	std::cout << "\nClient Tx: " << cmd;

	/*if (cmd == "G 1")
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
			client.connect_socket("192.168.1.3", 69420);
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
			client.connect_socket("192.168.1.3", 69420);
		}
	}*/
}


void getTopDown() {
	cv::Mat im;

	do {

		craigClient.tx_str("G 1");

		
		if (craigClient.rx_im(im)) {
			if (!im.empty()) {
				cv::imshow("rx", im);
				//std::cout << "im received\n";
			}
			else {
				std::cout << "blank image received\n";
			}
		}
		else {
			//std::cout << "no image received\n";
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

	} while (cv::waitKey(50) != 'q');
}

void manual() {
	std::cout << "\n***********************************";
	std::cout << "\n* Manual Controls";
	std::cout << "\n***********************************";

	craigClient.connect_socket("192.168.1.100", 46991);
	std::thread t1(&getTopDown);
	t1.detach();

	while (!(GetKeyState('P') & 0x8000)) {
		if (GetKeyState('W') & 0x8000)
		{
			send_command(piClient, std::to_string(FORWARD));
		}
		else if (GetKeyState('A') & 0x8000)
		{
			send_command(piClient, std::to_string(LEFT));
		}
		else if (GetKeyState('D') & 0x8000)
		{
			send_command(piClient, std::to_string(RIGHT));
		}
		else if (GetKeyState('S') & 0x8000)
		{
			send_command(piClient, std::to_string(BACK));
		}
		else if (GetKeyState('G') & 0x8000)
		{
			send_command(piClient, std::to_string(SHOOT));
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}


}

int main(int argc, char* argv[])
{
	piClient.connect_socket("192.168.1.3", 54420);

	//while (1);

	int cmd = -1;
	do
	{
		print_menu();
		std::cin >> cmd;
		switch (cmd)
		{
			//case 1: test_com(); break;
		case 2: manual(); cmd = NULL; break;
			case 3: test_menu(); cmd = NULL; break;
			//case 4: do_clientserver(); break;
		}
	} while (cmd != 'X');
}
