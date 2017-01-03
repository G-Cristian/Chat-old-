#include "TCPDefinitions.h"

#if defined(WIN32) || defined(_WIN32)
//We are in windows

Result listenSocket(Socket listenSckt){
	Result result;
	result.message = "Listen ok.\n";
	result.type = R_OK;

	if (listen(listenSckt, SOMAXCONN) == SOCKET_ERROR){
		std::ostringstream s;
		s << "Listen failed with error: " << WSAGetLastError() << "\n";
		result.message = s.str();
		result.type = R_ERROR;
	}

	return result;
}

Result acceptClient(Socket listenSocket, Socket *clientSocket){
	Result result;
	result.message = "Accept ok.\n";
	result.type = R_OK;

	*clientSocket = accept(listenSocket, NULL, NULL);
	if (*clientSocket == INVALID_SOCKET)
	{
		std::ostringstream s;
		s << "Accept failed with error: " << WSAGetLastError() << "\n";
		result.message = s.str();
		result.type = R_ERROR;
	}

	return result;
}

#else
//We are not in windows

#error "OS not supported."

#endif

Result receive(Socket clientSocket, Commands::CommandType* command, string *msg){
	Result result;

	return result;
}

Result sendStatus(Socket clientSocket, StatusResults::StatusResultType statusResult){
	Result result;

	return result;
}

Result sendClientsInGroup(Socket clientSocket, const string clientsNames) {
	Result result;

	return result;
}

Result sendMsg(Socket clientSocket, string &msg){
	Result result;

	return result;
}

Result sendClientJoinedGroup(Socket clientSocket, string groupName, string newClientName) {
	Result result;

	return result;
}