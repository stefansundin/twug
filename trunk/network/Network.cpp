#include "Network.h"

Network::Network()
{
}
Network::~Network()
{
	std::map<int, buffer_t>::iterator bitr;
	for(bitr = m_buffers.begin(); bitr != m_buffers.begin(); bitr++)
	{
		close(bitr->first);
	}
}

bool Network::getMessage(message_t &p_msg)
{
	if(!m_messages.empty())
	{
		p_msg = m_messages.front();
		m_messages.pop();
		return true;
	}
	return false;
}
//function to be called once the server has selected a client socket or whenever on the clientside
bool Network::updateBuffer(int p_socket)
{
	//recived data
	char *recv_buffer = new char[2048];
	int recv_length = recv(p_socket, (void*)recv_buffer, 2048, 0);
	if(recv_length == -1)
	{
		report_error(strerror(errno));
		return false;
	}
	else if(recv_length == 0)
	{
		log_this("disconnected");
		return false;
	}

	//create a temp variable big enough to contain the new and old data
	char *new_buffer = new char[m_buffers[p_socket].m_length+recv_length];
	memcpy(new_buffer, m_buffers[p_socket].m_buffer, m_buffers[p_socket].m_length);
	memcpy(new_buffer+m_buffers[p_socket].m_length, recv_buffer, recv_length);

	//update the buffer
	m_buffers[p_socket].m_length += recv_length;
	m_buffers[p_socket].m_buffer = new_buffer;

	if(m_buffers[p_socket].m_length >= sizeof(header))
	{
		printf("we've got a message!\n");
		int message_length;
		memcpy(&message_length, m_buffers[p_socket].m_buffer+sizeof(char)*5+sizeof(short)*2, sizeof(int));
		printf("message length: %d\n", message_length);

		if(m_buffers[p_socket].m_length-sizeof(header) >= message_length)
		{
			//get the message
			int header_message_length = sizeof(header)+message_length;
			char *message = new char[header_message_length];
			memcpy(message, m_buffers[p_socket].m_buffer, header_message_length);

			//remove message from the buffer
			int new_length = m_buffers[p_socket].m_length - header_message_length;
			new_buffer = new char[new_length];
			memcpy(new_buffer, m_buffers[p_socket].m_buffer, header_message_length);
			m_buffers[p_socket].m_length = new_length;
			m_buffers[p_socket].m_buffer = new_buffer;

			//add the new message to the message queue
			short message_type;
			memcpy(&message_type, message+sizeof(char)*5+sizeof(short), sizeof(short));
			message_t m;
			m.m_socket = p_socket;
			m.m_data = Data(message_type, message, message_length);
			m_messages.push(m);
		}
	}

	return true;
}


void Network::sendData(int p_socket, Data &p_data)
{
	header h;
	memcpy(h.id, "TWUG", sizeof(char)*5);
	h.version = NETWORK_VERSION;
	h.message_type = p_data.getType();
	h.message_length = p_data.getLength();

	send(p_socket, (void*)&h, sizeof(header), 0);
	send(p_socket, p_data.getData(), p_data.getLength(), 0);
}
