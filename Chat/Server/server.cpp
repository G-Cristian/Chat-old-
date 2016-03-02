#include "server.h"
#include "result.h"
#include <iostream>
#include <string>
#include <utility>

using namespace std;

const string Server::GlobalGroupName = "_GLOBALGROUP";

Server::Server(Socket listenSocket)
{
}

Server::~Server()
{
}

int Server::threadStart(){
	Result waitResult;
	Result signalResult;
	waitResult = mutexWait(m_threadsCountMutex);
	m_threadsCount++;
	signalResult = mutexSignal(m_threadsCountMutex);

#if defined(_DEBUG) || !defined(NDEBUG)
	string msg = "";
	if (waitResult.type != R_OK){
		msg = waitResult.message;

		if (signalResult.type != R_OK){
			msg.append("\n|____");
			msg.append(signalResult.message);
		}

		showMessage(msg);
	}
	else if (signalResult.type != R_OK){
		msg = signalResult.message;

		showMessage(msg);
	}
#endif

	return m_threadsCount;
}

int Server::threadFinish() {
	Result waitResult;
	Result signalResult;
	waitResult = mutexWait(m_threadsCountMutex);
	m_threadsCount--;
	signalResult = mutexSignal(m_threadsCountMutex);

#if defined(_DEBUG) || !defined(NDEBUG)
	string msg = "";
	if (waitResult.type != R_OK){
		msg = waitResult.message;

		if (signalResult.type != R_OK){
			msg.append("\n|____");
			msg.append(signalResult.message);
		}

		showMessage(msg);
	}
	else if (signalResult.type != R_OK){
		msg = signalResult.message;

		showMessage(msg);
	}
#endif
	return m_threadsCount;
}

void Server::showMessage(string message){
	Result waitResult;
	Result signalResult;
	waitResult = mutexWait(m_coutMutex);
	cout << message << "\n";
	signalResult = mutexSignal(m_coutMutex);

	string msg = "";
	if (waitResult.type != R_OK){
		msg = waitResult.message;

		if (signalResult.type != R_OK){
			msg.append("\n|____");
			msg.append(signalResult.message);
		}

		cerr << msg << "\n";
	}
	else if (signalResult.type != R_OK){
		msg = signalResult.message;

		cerr << msg << "\n";
	}
}

Result Server::start(){
	Result res;
	res.type = R_OK;
	res.message = "Start OK";

	Group *globalGroup = new Group(NULL, GlobalGroupName);
	m_groups.insert(std::pair<GroupsKey, Group*>(globalGroup->getName(), globalGroup));

	Result listenResult = listenSocket(m_listenSocket);
	if (listenResult.type != R_OK){
		res.type = R_ERROR;
		res.message = listenResult.message;

		return res;
	}

	//accepts clients
	Result acceptResult;
	while(!criticalError){
		Socket clientSocket = INVALID_SOCKET;
		acceptResult = acceptClient(m_listenSocket, &clientSocket);
		cout << acceptResult.message;
		if (acceptResult.type == R_OK){
			ThreadData threadData;
			threadData.server = this;
			threadData.clientSocket = clientSocket;
			threadStart();
			//TODO
			/*Create thread, use listener as function and pass this and clientSocket as arguments*/
		}
	}

	//TODO
	//use conditional variable (conditional semaphore) in order to wait until all threads finish.
	//Then I can clean thread information and exit this method.
	//REMEMBER: All threads must validate server.criticalError and exit when this values true.
	/*Destroy data used when creating threads*/

	return res;
}

Result Server::createGroup(Client *client, string groupName){
	Result result;
	result.type = R_OK;
	result.message = "Group ";
	result.message.append(groupName);
	result.message.append(" added for client ");
	result.message.append(client->getName());
	result.message.append(".");

	Group *group = new Group(client, groupName);

	//wait in case someone is using the map
	Result waitResult = mutexWait(m_groupsMutex);

	if (waitResult.type == R_OK){
		//mutexWait succeeded

		//map insert result
		std::pair<Groups::iterator, bool> ret = m_groups.insert(std::pair<GroupsKey,Group*>(groupName, group));

		//release mutex
		Result signalResult = mutexSignal(m_groupsMutex);
		if (signalResult.type == R_OK){
			if (!ret.second){
				//group name already exists for the client
				result.type = R_WARNING;
				result.message = "Group name \'";
				result.message.append(groupName);
				result.message.append("\' already exists for the client \'");
				result.message.append(client->getName());
				result.message.append("\'.");

				delete group;
			}
			else{
				//ok, add the group to the client who created it
				client->addGroup(group);
			}
		}
		else{
			//error in mutexSignal
			//possibly the thread does not own the mutex object

			result.type = R_ERROR;
			result.message = signalResult.message;
			result.message.append("\n |__ Couldn't release mutex.");

			if (!ret.second)
			{
				result.message.append("\n     |__ Group name \'");
				result.message.append(groupName);
				result.message.append("\' already exists for the client \'");
				result.message.append(client->getName());
				result.message.append("\'.");

				delete group;
			}
			else
			{
				client->addGroup(group);

				result.message.append("\n     |__ Group ");
				result.message.append(groupName);
				result.message.append(" added for client ");
				result.message.append(client->getName());
				result.message.append(".");
			}
		}
	}
	else{
		//mutexWait error
		result.type = R_ERROR;
		result.message = waitResult.message;
		result.message.append("\n | __ Group ");
		result.message.append(groupName);
		result.message.append(" not added.");

		delete group;
	}

	return result;
}

Result Server::joinGroup(Client *client, string groupName){
	Result result;

	Result waitResult = mutexWait(m_groupsMutex);
	if (waitResult.type == R_OK){
		//mutexWait succeeded
		//Note:I protect when the member is added because otherwise the group could be removed while someone is joining

		Groups::iterator groupIter = m_groups.find(groupName);
		if (groupIter != m_groups.end()){
			//ok, add member

			result = groupIter->second->addMember(client);
			if (result.type == R_OK){
				//tell all other member of the group about the existence of this new memeber
				//and build the string, with the group name and members names, to be sent to this new client.
				Group *group = groupIter->second;
				string extraData = groupName;
				extraData.append("|");
				waitResult = mutexWait(group->m_membersMutex);
				if (waitResult.type == R_OK){
					map<string, Client*>::const_iterator it = group->getMembers().begin();
					
					Client* c = NULL;
					for (; it != group->getMembers().end(); it++){
						c = it->second;
						extraData.append(c->getName());
						extraData.append("|");
						Result sendResult = sendClientJoinedGroup(c->getSocket, groupName, client->getName());
						result.message.append("\n        |__");
						result.message.append(sendResult.message);
					}

					Result sendResult = sendClientsInGroup(client->getSocket(), extraData);

					result.message.append("\n               |__");
					result.message.append(sendResult.message);

					Result signalResult = mutexSignal(group->m_membersMutex);
					if (signalResult.type != R_OK){
						//error in mutexSignal
						//possibly the thread does not own the mutex object
						result.type = R_ERROR;
						result.message.append("\n      |__");
						result.message.append(signalResult.message);
						result.message.append("\n         |__ Couldn't release mutex.");
					}
				}
				else{
					result.type = R_ERROR;
					result.message.append("\n        |__");
					result.message.append(waitResult.message);
				}
			}
		}
		else{
			//element exists
			
			result.type = R_WARNING;
			result.message = "Group .";
			result.message.append(groupName);
			result.message.append(" does not exists.");
		}

		Result signalResult = mutexSignal(m_groupsMutex);
		if (signalResult.type != R_OK){
			//error in mutexSignal
			//possibly the thread does not own the mutex object

			result.type = R_ERROR;
			result.message.append("\n |__");
			result.message.append(signalResult.message);
			result.message.append("\n     |__ Couldn't release mutex.");
		}
	}
	else{
		//mutexWait error
		result.type = R_ERROR;
		result.message = waitResult.message;
		result.message.append("\n | __ Client ");
		result.message.append(client->getName());
		result.message.append(" not added to group ");
		result.message.append(groupName);
		result.message.append(".");
	}

	return result;
}

Result Server::leaveGroup(Client *client, string groupName){
	Result result;

	Result waitResult = mutexWait(m_groupsMutex);
	if (waitResult.type == R_OK){
		//mutexWait succeeded
		//Note:I protect when the member is leaving because otherwise if he/she is the creator,
		//the group must be removed and someone could be joining.

		Groups::iterator group = m_groups.find(groupName);
		if (group != m_groups.end()){
			//ok, leave group

			//if this client is the creator of the group, I must remove the group.
			if (client == group->second->getCreator()){
				//TODO
			}
			else{
				//TODO
			}
		}
		else{
			//element exists
		
			result.type = R_WARNING;
			result.message = "Group .";
			result.message.append(groupName);
			result.message.append(" does not exists.");
		}

		Result signalResult = mutexSignal(m_groupsMutex);
		if (signalResult.type != R_OK){
			//error in mutexSignal
			//possibly the thread does not own the mutex object

			result.type = R_ERROR;
			result.message.append("\n |__");
			result.message.append(signalResult.message);
			result.message.append("\n     |__ Couldn't release mutex.");
		}
	}
	else{
		//mutexWait error
		result.type = R_ERROR;
		result.message = waitResult.message;
		result.message.append("\n | __ Client ");
		result.message.append(client->getName());
		result.message.append(" didn't leave the group ");
		result.message.append(groupName);
		result.message.append(".");
	}

	return result;
}

pair<Result, Client*> Server::addClient(string name, Socket clientSocket){
	Result result;
	result.type = R_OK;
	result.message = "Client ";
	result.message.append(name);
	result.message.append( " added.");

	Client *client = new Client(name, clientSocket, this);

	//waits in case that someone is using the map
	Result waitResult = mutexWait(m_clientsMutex);

	if (waitResult.type == R_OK)
	{
		//mutexWait succeeded

		//map insert result
		std::pair<Clients::iterator, bool> ret = m_clients.insert(std::pair<ClientsKey, Client*>(name, client));
		//release mutex
		Result signalResult = mutexSignal(m_clientsMutex);
		if (signalResult.type == R_OK)
		{
			if (!ret.second)
			{
				//client name already exists
				result.type = R_WARNING;
				result.message = "Client name already exists.";

				delete client;
				client = NULL;
			}
		}
		else
		{
			//error in mutexSignal
			//possibly the thread does not own the mutex object

			result.type = R_ERROR;
			result.message = signalResult.message;
			result.message.append("\n |__ Couldn't release mutex.");
			
			if (!ret.second)
			{
				result.message.append("\n     |__ Client name \'");
				result.message.append(name);
				result.message.append("\'already exists.");

				delete client;
				client = NULL;
			}
			else
			{
				result.message.append("\n     |__ Client ");
				result.message.append(name);
				result.message.append(" added.");
			}
				
		}
	}
	else
	{
		//mutexWait error
		result.type = R_ERROR;
		result.message = waitResult.message;
		result.message.append("\n | __ Client ");
		result.message.append(name);
		result.message.append(" not added.");

		delete client;
		client = NULL;
	}

	return pair<Result, Client*>(result, client);
}

Result Server::leaveChat(Client *client){
	Result result;

	return result;
}

Result Server::listenClient(Client *client){
	Result result;

	return result;
}


#if defined(WIN32) || defined(_WIN32)
DWORD listener(LPVOID param)
#else
void *listener(void * param)
#endif
{
	Server::ThreadData* data = (Server::ThreadData*)param;
	Server* server = data->server;
	Socket clientSocket = data->clientSocket;
	Client * client = NULL;

	Commands::CommandType command;
	string msg;
	bool clientCreated = false;
	if (!server->criticalError){
		do{
			//receive name
			Result receiveResult = receive(clientSocket, &command, &msg);
			ShowMessage(receiveResult.message);
			Result sendResult;
			if (receiveResult.type == R_ERROR){
				sendResult = sendStatus(clientSocket, StatusResults::STATUS_RECEIVE_FAIL);
				server->criticalError = true;
			}
			else if (receiveResult.type == R_WARNING){
				//possibly the client closed connection without sending CLIENT_LEAVES_CHAT
				break;
			}
			else{
				if (command == Commands::CLIENT_CONNECTS){
					//name received
					pair<Result, Client*> addClientResult = server->addClient(msg, clientSocket);
					ShowMessage(addClientResult.first.message);
					if (addClientResult.first.type == R_ERROR)
					{
						//critical error
						server->criticalError = true;
					}
					else if (addClientResult.first.type == R_WARNING){
						//client name already exists
						sendResult = sendStatus(clientSocket, StatusResults::STATUS_CLIENT_NAME_EXISTS);
						ShowMessage(sendResult.message);
					}
					else{
						Result joinGroupResult = server->joinGroup(addClientResult.second, Server::GlobalGroupName);
						ShowMessage(joinGroupResult.message);
						if (joinGroupResult.type != R_OK){
							//critical error
							server->criticalError = true;
						}
						else{
							//client created and added to global group
							sendResult = sendStatus(clientSocket, StatusResults::STATUS_OK);
							client = addClientResult.second;
							clientCreated = true;

							ShowMessage(sendResult.message);
						}
					}
				}
			}
			

		} while (command != Commands::CLIENT_LEAVES_CHAT && !clientCreated && !server->criticalError);
	}

	if (server->criticalError){
		Result sendResult = sendStatus(clientSocket, StatusResults::STATUS_SERVER_CRITICAL_ERROR);

		ShowMessage(sendResult.message);
	}
	else if (clientCreated){
		Result listenClientResult = server->listenClient(client);

		ShowMessage(listenClientResult.message);
	}

	server->threadFinish();
#if defined(WIN32) || defined(_WIN32)
	if (server->criticalError)
		return 1;
	else
		return 0;
#else
	//not windows
	return NULL;
#endif
}