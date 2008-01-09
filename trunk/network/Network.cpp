#include "Network.h"

Network::Network()
{
	//m_socket = tcp socket;
}
Network::~Network()
{
}

int Network::connect(std::string p_address, int p_port)
{
	return 0;
}
int Network::disconnect()
{
	return 0;
}

int sendData(Data p_data)
{
	header h;
	h.id = "BULL";
	h.version = NETWORK_VERSION;
	h.message_type = p_data.getType();
	h.message_length = p_data.getLength();

	m_socket.send(header);
	m_socket.send(p_data.getData().c_str());

	return 0;
}
int getData(Data &p_data)
{
	//socket.read() to a buffer
	//get a whole message from the buffer if there is one
	//process it
	if(/*buffer has complete message*/true)
	{
		the_mentioned_complete_message = Data(123, "sender", 456); //temp
		if(p_data = the_mentioned_complete_message)
			return 1;
		else
			return -1;
	}
	else
		return 0;
}
