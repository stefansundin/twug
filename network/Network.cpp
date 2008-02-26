#include "Network.h"

Network::Network()
{
	m_messages = new std::queue<Message>();
}
Network::~Network()
{
	std::map<int, Buffer>::iterator bitr;
	for(bitr = m_buffers.begin(); bitr != m_buffers.end(); bitr++)
	{
		close(bitr->first);
	}
	delete m_messages;
}

bool Network::getMessage(Message &p_msg)
{
	if(!m_messages->empty())
	{
		p_msg = m_messages->front();
		m_messages->pop();
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
	printf("m_buffers[p_socket].getLength(): \"%d\"\n", m_buffers[p_socket].getLength());
	printf("recv_length: \"%d\"\n", recv_length);
	char *new_buffer = new char[m_buffers[p_socket].getLength()+recv_length];
	memcpy(new_buffer, m_buffers[p_socket].getBuffer(), m_buffers[p_socket].getLength());
	memcpy(new_buffer+m_buffers[p_socket].getLength(), recv_buffer, recv_length);

	//update the buffer
	printf("setting buffer length to: \"%d\" + \"%d\"\n", m_buffers[p_socket].getLength(), recv_length);
	m_buffers[p_socket].setLength(m_buffers[p_socket].getLength() + recv_length);
	m_buffers[p_socket].setBuffer(new_buffer);

	if(m_buffers[p_socket].getLength() >= sizeof(header))
	{
		printf("got part of a message\n");

		header temp_header;
		memcpy(&temp_header, m_buffers[p_socket].getBuffer(), sizeof(header));
		printf("-----header-----\n");
		printf("temp_header.id = %s\n", temp_header.id);
		printf("temp_header.version = %d\n", temp_header.version);
		printf("temp_header.message_type = %d\n", temp_header.message_type);
		printf("temp_header.message_length = %d\n", temp_header.message_length);
		printf("----------------\n");

		if(m_buffers[p_socket].getLength()-sizeof(header) >= temp_header.message_length)
		{
			printf("got a complete message\n");
			printf("length: %d\n", m_buffers[p_socket].getLength()-sizeof(header));
			printf("lengthy: %d\n", temp_header.message_length);

			//get the message
			char *message = new char[temp_header.message_length];	//dont include the header now
			memcpy(message, m_buffers[p_socket].getBuffer()+sizeof(header), temp_header.message_length);

			//setup nice temp length variables
			int header_message_length = sizeof(header)+temp_header.message_length;
			int new_length = m_buffers[p_socket].getLength()-header_message_length;

			//setup the new buffer
			new_buffer = new char[new_length];
			memcpy(new_buffer, m_buffers[p_socket].getBuffer()+header_message_length, new_length);
			m_buffers[p_socket].setLength(new_length);
			m_buffers[p_socket].setBuffer(new_buffer);

			//add the new message to the message queue
			Message m;
			m.setSocket(p_socket);
			printf("message_type: %d\nmessage: %s\nmessage_length: %d\n", temp_header.message_type, message, temp_header.message_length);
			m.setData(new Data(temp_header.message_type, message, temp_header.message_length));
			m_messages->push(m);
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