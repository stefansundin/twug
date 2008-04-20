#include "Channel.h"

Channel::Channel()
{
	m_name = "no name";
	m_password = "mega_password";
}
Channel::Channel(std::string p_name, std::string p_password)
{
	m_name = p_name;
	m_password = p_password;
}
Channel::~Channel()
{
}


std::string Channel::getName() const
{
	return m_name;
}
std::string Channel::getPassword() const
{
	return m_password;
}


void Channel::setPassword(std::string p_password)
{
	m_password = p_password;
}

bool Channel::operator<(const Channel &p_rhs) const
{
	return p_rhs.m_name > m_name;
}

