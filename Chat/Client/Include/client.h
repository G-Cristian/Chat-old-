#ifndef _SERVER_CLIENT_H_
#define _SERVER_CLIENT_H_

#include "TCPDefinitions.h"
#include "ThreadsDefinitions.h"
#include <string>
#include <list>


using namespace std;

class Server;
class Group;

class Client{
public:
	Client(string name, Socket socket, Server *server);
	~Client();

	inline const string getName() const { return m_name; }
	inline const Socket getSocket() const { return m_socket; }
	//Adds a group to the groups container (assumes that the group doesn't already exist)	
	inline void addGroup(Group* group){ //Note: I don't protect this with a mutex because each client is bound to a single thread.
										m_groups.push_back(group); }
private:
	Server * m_server;
	string m_name;
	Socket m_socket;

	/*groups of which this client is a creator.
	It's used only to be able to delete the groups this client owns when the client is deleted,
	so I don't have to iterate all the server's groups
	*/
	 list<Group*> m_groups;
};

#endif