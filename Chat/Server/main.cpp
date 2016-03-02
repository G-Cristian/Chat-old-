#if defined(WIN32) || defined(_WIN32)
//We are in windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>

#pragma comment(lib, "Ws2_32.lib")
#else
//We are not in windows
#error "OS not supported."
#endif

#define DEFAULT_PORT "27015"

#include <stdio.h>
#include <conio.h>
#include <iostream>
#include "result.h"
#include "server.h"

using namespace std;

int main()
{
	int retCode = 0;
#if defined(WIN32) || defined(_WIN32)
	WSADATA wsaData;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0){
		printf("WSAStartup failed: %d \n", iResult);
		_getch();
		return 1;
	}

	struct addrinfo *result = NULL, *ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0){
		printf("getaddrinfo failed: %d \n", iResult);
		WSACleanup();
		_getch();
		return 1;
	}

	SOCKET listenSocket = INVALID_SOCKET;
	// Create a SOCKET for the server to listen for client connections
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET){
		printf("Error at socket(): %ld \n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		_getch();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR){
		printf("bind failed with error: %d \n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		_getch();
		return 1;
	}

	freeaddrinfo(result);
#else
	//We are not in windows
#error "OS not supported."
#endif

	Server *server = new Server(listenSocket);
	Result serverResult = server->start();

	if (serverResult.type != R_OK){
		cout << serverResult.message;
		retCode = 1;
	}

	delete server;

//cleanup
#if defined(WIN32) || defined(_WIN32)
	//windows
	closesocket(listenSocket);
	WSACleanup();
#else
	//We are not in windows
#error "OS not supported."
#endif

	_getch();
	return retCode;
}