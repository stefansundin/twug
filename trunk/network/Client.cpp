#include "Client.h"

Client::Client()
{
	m_name = "";
	m_socket = 0;
	m_privileges = PRIV_TALK | PRIV_TEXT | PRIV_ALL;	//remove PRIV_ALL later
}
Client::Client(std::string p_name, int p_socket)
{
	m_name = p_name;
	m_socket = p_socket;
	m_privileges = PRIV_TALK | PRIV_TEXT | PRIV_ALL;	//remove PRIV_ALL later
}
Client::Client(std::string p_name, int p_socket, int p_privileges)
{
	m_name = p_name;
	m_socket = p_socket;
	m_privileges = p_privileges;
}
Client::~Client()
{
}


std::string Client::getName() const
{
	return m_name;
}
int Client::getSocket() const
{
	return m_socket;
}
int Client::getPrivileges() const
{
	return m_privileges;
}

