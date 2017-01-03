#ifndef _SERVER_H_
#define _SERVER_H_

#include "ConstsAndGlobals.h"
#include "ThreadsDefinitions.h"
#include "TCPDefinitions.h"
#include "client.h"
#include "group.h"
#include "result.h"
#include "Counter.h"
#include <string>
#include <map>

using namespace std;

typedef string ClientsKey;
typedef map < ClientsKey, Client* > Clients;
typedef string GroupsKey;
typedef map < GroupsKey , Group* > Groups;  //the key is the group name

class Server
{
public:
	Server(Socket listenSocket);
	~Server();

	int threadStart();
	int threadFinish();

	//The method where the server listens to new connections.
	Result start();
	//Creates a new group only if there isn't a group with 'groupName' name.
	Result createGroup(Client *client, string groupName);
	/*The inviter sends an invitation to 'invitedClient' to join the 'groupName' group.
	If 'invitedClient' or 'groupName' group doesn't exist, the result is a warning.
	*/
	Result inviteClientToGroup(Client* inviter, string invitedClient, string groupName) const;
	//Joins the client to the group with 'groupName' name only if such group exists.
	Result joinGroup(Client *client, string groupName);
	/*Client leaves the group with 'groupName' name only if such group exists.
	  If client is the creator of the group, it must be removed.
	*/
	Result leaveGroup(Client *client, string groupName);
	//Add a new client when he connects.
	pair<Result, Client*> addClient(string name, Socket clientSocket);
	//Client leaves the chat.
	Result leaveChat(Client *client);
	//Function used in threads to listen to each client.
	Result listenClient(Client *client);

	//struct used to pass server and client socket information to new threads
	typedef struct{
		Socket clientSocket;
		Server* server;
	} ThreadData;

	//Global group name
	const static string GlobalGroupName;

	bool criticalError;
private:
	Socket m_listenSocket;
	Mutex m_clientsMutex;
	Mutex m_groupsMutex;

	Clients m_clients;
	Groups m_groups;
	Counter *m_threadsCount;
};

//TODO
//function that is passed to the thread created per client
//here the method listenClient() is called.
#if defined(WIN32) || defined(_WIN32)
DWORD listener(LPVOID param);
#else
void *listener(void * param);
#endif

#endif