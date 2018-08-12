#ifndef _CONSTS_AND_GLOBALS_H_
#define _CONSTS_AND_GLOBALS_H_

#include <string>

namespace Commands{

	typedef unsigned char CommandType;
	//Commands sent by client and received by server
	const CommandType CLIENT_CONNECTS = 0x01;
	const CommandType CLIENT_CREATES_GROUP = 0x02;
	const CommandType CLIENT_INVITES_CLIENT = 0x03;
	const CommandType CLIENT_JOINS_GROUP = 0x04;
	const CommandType CLIENT_LEAVES_GROUP = 0x05;
	const CommandType CLIENT_SENDS_MESSAGE = 0x06;
	const CommandType CLIENT_LEAVES_CHAT = 0x07;

	//Commands sent by server and received by client
	const CommandType SERVER_COMMAND_STATUS = 0x11;
	const CommandType SERVER_CLIENT_INVITED_CLIENT = 0x12;
	const CommandType SERVER_CLIENT_JOINED_GROUP = 0x13;
	const CommandType SERVER_CLIENT_LEFT_GROUP = 0x14;
	const CommandType SERVER_CLIENT_SENT_MESSAGE = 0x15;
	const CommandType SERVER_CLIENTS_IN_GROUP = 0x16;
}

namespace StatusResults{
	typedef unsigned char StatusResultType;
	//Status results sent when the Command is SERVER_COMMAND_STATUS
	const StatusResultType STATUS_OK = 0x01;
	const StatusResultType STATUS_CLIENT_NAME_EXISTS = 0x02;
	const StatusResultType STATUS_CLIENT_NAME_DOESNT_EXIST = 0x03;
	const StatusResultType STATUS_GROUP_NAME_EXISTS = 0x04;
	const StatusResultType STATUS_GROUP_NAME_DOESNT_EXIST = 0x05;
	const StatusResultType STATUS_RECEIVE_FAIL = 0x06;
	const StatusResultType STATUS_SERVER_CRITICAL_ERROR = 0x07;
}

#endif