#include "Client.h"

Client::Client()
{
	m_channel = NULL;
	m_username = "no_name_client";
	m_privileges = 0x0;
}
Client::Client(std::string p_username)
{
	m_channel = NULL;
	m_username = p_username;
	m_privileges = 0x0;
}
Client::~Client()
{
}


Channel* Client::getChannel()
{
	return m_channel;
}
std::string Client::getUsername()
{
	return m_username;
}
unsigned char Client::getPrivileges()
{
	return m_privileges;
}


void Client::setChannel(Channel *p_channel)
{
	m_channel = p_channel;
}
void Client::setUsername(std::string p_username)
{
	m_username = p_username;
}
void Client::setPrivileges(unsigned char p_privileges)
{
	m_privileges = p_privileges;
}

