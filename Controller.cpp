#include "Controller.h" 

connectedClient::connectedClient(char* setName, char* setIp, unsigned int setClientId)
{

	this->name = setName;
	this->ipAddress = setIp;
	this->id = setClientId;
};

void __cdecl connectedClient::setIsActive(char isActiveBool)
{

	this->isActive = isActiveBool;
};

//-----------------------------------------------------------------------------------------------

void __cdecl sendInfoToClient(unsigned int clientId, char* byteBuffer, int size)
{

	send(clientId, byteBuffer, size, NULL);
	Sleep(5);
	return;
};

void  __cdecl outputColor(int colorCode)
{
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
	return;
};

void __cdecl Controller::recvThings(connectedClient newClient, std::vector<connectedClient>* _allConnectedClients)
{
	char byteBuffer[MAX_BUF];
	zeroBuffer(byteBuffer, MAX_BUF);
	std::string stringBuffer;
	while (1)
	{
		int recievedBytes = recv(newClient.id, byteBuffer, MAX_BUF, NULL);

		if (recievedBytes == NULL || recievedBytes == SOCKET_ERROR)
		{
			int i = 0;
			for (auto& cClient : *_allConnectedClients)
			{
				if (cClient.id == newClient.id)
				{

					_allConnectedClients->erase(_allConnectedClients->begin() + i);

					outputColor(0xC);
					std::cout << newClient.ipAddress << " has disconnected, client: " << i << std::endl;
					outputColor(0x7);
					std::cout << ">>> ";
				};
				i++;
			};
			break;
		}
		else
		{
			stringBuffer = std::string(byteBuffer, recievedBytes);

			int currentVectorPos;
			for (unsigned int i = 0; i <= _allConnectedClients->size() - 1; i++)
			{
				if ((*_allConnectedClients)[i].ipAddress == newClient.ipAddress && (*_allConnectedClients)[i].id == newClient.id)
				{

					currentVectorPos = i;
				};
			};
			if ((*_allConnectedClients)[currentVectorPos].isActive == 1)
			{

				//--Do other things to the bytes. I can make it interpret stuff for file transfer etc.
				char c = stringBuffer.c_str()[0];

				stringBuffer = stringBuffer.substr(1);

				switch (c)
				{
				case text:
				{

					std::cout << "\b\b\b\b " << stringBuffer << std::endl;
					break;
				};
				case file:
				{

					std::ofstream outStream(stringBuffer, std::ios::binary);

					while (1)
					{
						char* buf = new char[MAX_BUF];
						zeroBuffer(buf, MAX_BUF);

						int fileByteLength = recv(newClient.id, buf, MAX_BUF, NULL);

						outStream.write(buf, fileByteLength);

						if (fileByteLength < MAX_BUF)
						{

							delete[] buf;
							break;
						};


						delete[] buf;
					};

					outStream.close();
					break;
				};
				default:
				{

					break;
				};
				};
			};
		};
	};
};

void __cdecl Controller::listenForClients(int listeningServerSocket, unsigned int* _socketPort, std::string* _ipv4ADDR, std::vector<connectedClient>* _allConnectedClients, std::vector<std::thread>* _threads)
{

	sockaddr_in socketAddress;
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(*_socketPort);
	inet_pton(AF_INET, _ipv4ADDR->c_str(), &socketAddress.sin_addr);
	while (1)
	{
		//-- Two colons to fix multithreading wackiness
		::bind(listeningServerSocket, (sockaddr*)&socketAddress, sizeof(socketAddress));
		listen(listeningServerSocket, SOMAXCONN);

		sockaddr_in clientSocketAddress;
		int clientSocketSize = sizeof(clientSocketAddress);

		unsigned int clientId = accept(listeningServerSocket, (sockaddr*)&clientSocketAddress, &clientSocketSize);

		char computerName[216], clientIp[216], serverPort[216];

		zeroBuffer(computerName, 216);
		zeroBuffer(clientIp, 216);
		zeroBuffer(serverPort, 216);

		char nameInformation = getnameinfo((sockaddr*)&clientSocketAddress, sizeof(clientSocketAddress), computerName, NI_MAXHOST, serverPort, NI_MAXSERV, NULL);
		inet_ntop(AF_INET, &clientSocketAddress.sin_addr, clientIp, NI_MAXHOST);

		if (nameInformation == 0)
		{

			outputColor(0xA);
			std::cout << computerName << " has connected to port " << serverPort << ". IP: " << clientIp << std::endl;
			outputColor(0x7);
			std::cout << ">>> ";

			char sendBuffer[MAX_BUF] = "Successfully connected to the server!";
			sendInfoToClient(clientId, sendBuffer, MAX_BUF);

			connectedClient newClient(computerName, clientIp, clientId);

			_allConnectedClients->push_back(newClient);

			_threads->push_back(std::thread(recvThings, newClient, _allConnectedClients));
		}
		else
		{
			outputColor(0xA);
			std::cout << computerName << " has connected to port " << ntohs(clientSocketAddress.sin_port) << ". IP: " << clientIp << std::endl;
			outputColor(0x7);
			std::cout << ">>> ";

			char sendBuffer[MAX_BUF] = "Successfully connected to the server!";

			sendInfoToClient(clientId, sendBuffer, MAX_BUF);
		};
	};
};
