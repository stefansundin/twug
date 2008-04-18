#include "Message.h"

Message::Message()
{
	m_socket = 0;
}
Message::~Message()
{
}


int Message::getSocket()
{
	return m_socket;
}
Data Message::getData()
{
	return m_data;
}


void Message::setSocket(int p_socket)
{
	m_socket = p_socket;
}
void Message::setData(Data p_data)
{
	m_data = p_data;
}

