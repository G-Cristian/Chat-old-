#ifndef _TCPDEFINITIONS_H_
#define _TCPDEFINITIONS_H_

#include "ConstsAndGlobals.h"
#include "result.h"
#include <sstream>


//Length of the buffer to receive/send data from/to clients
//TOTAL = Size of the command in bytes + number of characters that can be sent when chatting + padding
//512 = sizeof(CommandType) + sizeof(char)*256 + padding
//CommandType = defined in ConstsAndGlobals.h
//the extra space is useful when an error message must be sent.
#define DEFAULT_BUFLEN 512

#if defined(WIN32) || defined(_WIN32)
//We are in windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

#define Socket SOCKET

#else
//We are not in windows

#error "OS not supported."

//TODO define Scoket as the socket definition in this OS


#endif

Result listenSocket(Socket listenSckt);
Result acceptClient(Socket listenSocket, Socket *clientSocket);

Result receive(Socket clientSocket, Commands::CommandType* command, string *msg);

Result sendStatus(Socket clientSocket, StatusResults::StatusResultType statusResult);
Result sendClientsInGroup(Socket clientSocket, const string clientsNames);
Result sendMsg(Socket clientSocket, string &msg);
Result sendClientJoinedGroup(Socket clientSocket, string groupName, string newClientName);

#endif