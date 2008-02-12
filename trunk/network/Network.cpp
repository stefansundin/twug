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

	printf("recv() got: \"%s\" (%d bytes)\n", recv_buffer, recv_length);

	//create a temp variable big enough to contain the new and old data
	char *new_buffer = new char[m_buffers[p_socket].m_length+recv_length];
	memcpy(new_buffer, m_buffers[p_socket].m_buffer, m_buffers[p_socket].m_length);
	memcpy(new_buffer+m_buffers[p_socket].m_length, recv_buffer, recv_length);

	//update the buffer
	m_buffers[p_socket].m_length += recv_length;
	m_buffers[p_socket].m_buffer = new_buffer;

	if(m_buffers[p_socket].m_length >= sizeof(header))
	{
		printf("got part of a message\n");

		header temp_header;
		memcpy(&temp_header, m_buffers[p_socket].m_buffer, sizeof(header));
		printf("-----header-----\n");
		printf("temp_header.id = %s\n", temp_header.id);
		printf("temp_header.version = %d\n", temp_header.version);
		printf("temp_header.message_type = %d\n", temp_header.message_type);
		printf("temp_header.message_length = %d\n", temp_header.message_length);
		printf("----------------\n");

		if(m_buffers[p_socket].m_length-sizeof(header) >= temp_header.message_length)
		{
			printf("got a complete message\n");

			//get the message
			char *message = new char[sizeof(header)+temp_header.message_length];
			memcpy(message, m_buffers[p_socket].m_buffer, sizeof(header)+temp_header.message_length);

			//remove message from the buffer
			int new_length = m_buffers[p_socket].m_length - sizeof(header)+temp_header.message_length;
			new_buffer = new char[new_length];
			memcpy(new_buffer, m_buffers[p_socket].m_buffer, sizeof(header)+temp_header.message_length);
			m_buffers[p_socket].m_length = new_length;
			m_buffers[p_socket].m_buffer = new_buffer;

			//add the new message to the message queue
			char *message_no_header[temp_header.message_length-sizeof(header)];

			message_t m;
			m.m_socket = p_socket;
			m.m_data = Data(temp_header.message_type, message, temp_header.message_length);
			m_messages.push(m);
		}
	}

	return true;
}


void Network::sendData(int p_socket, Data &p_data)
{
	header h;
	memcpy(h.id, "TWUG", sizeof(char)*4);		//TODO: restore this
	memcpy(h.id+4, "0", sizeof(char)*1);
	h.version = NETWORK_VERSION;
	h.message_type = p_data.getType();
	h.message_length = p_data.getLength();

	send(p_socket, (void*)&h, sizeof(header), 0);
	send(p_socket, p_data.getData(), p_data.getLength(), 0);
}
