#ifndef _GROUP_H_
#define _GROUP_H_

#include "TCPDefinitions.h"
#include "ThreadsDefinitions.h"
#include "result.h"
#include <string>
#include <map>

using namespace std;

class Server;
class Client;

class Group
{
public:
	Group(Client *creator, string name);
	//removes members and closes the mutex.
	~Group();

	const Client* getCreator() const;
	string getName() const;

	Result addMember(Client *member);
	Result removeMember(Client *member);
	const map<string, Client*>& getMembers()const{ return m_members; }

	Mutex m_membersMutex;
	
private:
	/*the creator is used because when the client who created the group disconnects,
	  I delete the groups he/she created.
	*/
	Client* m_creator;
	string m_name;
	map<string,Client*> m_members;
	
};

#endif