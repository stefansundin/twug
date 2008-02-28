#include "Client.h"

Client::Client()
{
	m_socket = 0;
	m_username = "";
	m_privileges = 0x0;
}
Client::~Client()
{
}

int Client::getSocket()
{
	return m_socket;
}
std::string Client::getUsername()
{
	return m_username;
}
unsigned char Client::getPrivileges()
{
	return m_privileges;
}

void Client::setSocket(int p_socket)
{
	m_socket = p_socket;
}
void Client::setUsername(std::string p_username)
{
	m_username = p_username;
}
void Client::setPrivileges(unsigned char p_privileges)
{
	m_privileges = p_privileges;
}

