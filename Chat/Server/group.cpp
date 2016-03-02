#include "group.h"
#include "client.h"

Group::Group(Client *creator, string name){
	//TODO set creator
	//TODO add creator to members
}

Group::~Group(){
}

const Client* Group::getCreator() const
{
	return m_creator;
}

string Group::getName() const
{
	return m_name;
}

Result Group::addMember(Client *member){
	Result result;
	result.type = R_OK;
	result.message = "Client ";
	result.message.append(member->getName());
	result.message.append(" joined group ");
	result.message.append(m_name);
	result.message.append(".");

	return result;
}

Result Group::removeMember(Client *member){
	Result result;
	result.type = R_OK;
	result.message = "Client ";
	result.message.append(member->getName());
	result.message.append(" left group ");
	result.message.append(m_name);
	result.message.append(".");

	return result;
}