#include "Network.h"

Network::Network()
{
	m_connected = false;
	m_message_in_buffer = false;
	m_socket = Socket(AF_INET, SOCK_STREAM);
}
Network::~Network()
{
	if(m_connected)
		disconnect();
}

int Network::connect(std::string p_address, int p_port)
{
	int status = m_socket.connect(p_address, p_port);
	if(status == 0)
		m_connected = true;
	return status;
}
int Network::disconnect()
{
	return m_socket.close();
}

int Network::sendData(Data p_data)
{
	header h;
	h.id = "BULL";
	h.version = NETWORK_VERSION;
	h.message_type = p_data.getType();
	h.message_length = p_data.getLength();

	m_socket.send(h, sizeof(h));
	m_socket.send(p_data.getData(), p_data.getLength());

	return 0;
}
int Network::getData(Data &p_data)
{
	//socket.read() to a buffer
	//get a whole message from the buffer if there is one
	//process it
	if(m_message_in_buffer)
	{
//		the_mentioned_complete_message		Data(123, "sender");
	}
	else
		return 0;
}
