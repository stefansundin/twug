#include "Network.h"

Network::Network()
{
	m_connected = false;
}
Network::~Network()
{
	if(m_connected)
	{
		std::buffer<int, buffer> bitr;
		for(bitr = m_buffers.begin(); bitr != m_buffers.begin(); bitr++)
		{
			close(bitr->first);
		}
	}
}

/* the client can subclass this class and add these functions to that subclass instead
connect()
disconnect()
void Network::loginRequest(std::string p_user, std::string p_password)
{
	fill(p_user, 20);
	fill(p_password, 20);
	std::string temp = p_user + p_password;

	Data data = Data(OUT_LOGIN_REQUEST, temp.c_str(), temp.size());
	sendData(data);
}
void Network::logout()
{
	Data data = Data(OUT_LOGOUT, "", 0);
	sendData(data);
}
*/


bool Network::getMessage(Data &p_data)
{
	if(!m_message_buffer.empty())
	{
		p_data = m_message_buffer.front();
		m_message_buffer.pop();
		return true;
	}
	return false;
}
void Network::updateBuffer(int p_socket)
{
	//recived data
	char *recv_buffer;
	int recv_length = recv(p_socket, recv_recived, 2048, 0);

	//make the next statements easier to read
	char *buffer = m_buffers[p_socket].m_buffer;
	int buffer_length = m_buffers[p_socket].m_length;

	//create a temp variable big enough to contain the new and old data
	char *temp = new char[buffer_length+length];
	memcpy(temp, buffer, buffer_length);
	memcpy(temp+buffer_length, recv_buffer, recv_length);

	//update the buffer
	m_buffers[p_socket].m_length += recv_length;
	m_buffers[p_socket].m_buffer = temp;

	if(m_buffers[p_socket].m_length >= sizeof(header))
	{
		int message_length;
		memcpy(&message_length, m_buffer+sizeof(char)*5+sizeof(short)*2, sizeof(int));

		if(m_buffers[p_socket].m_length-sizeof(header) >= message_length)
		{
			//get the message
			int header_message_length = sizeof(header)+message_length;
			char *message = new char[header_message_length];
			memcpy(message, m_buffers[p_socket].m_buffer, header_message_length);

			//remove message from the buffer
			int new_length = m_buffers[p_socket].m_length - header_message_length;
			temp = new char[new_length];
			memcpy(temp, m_buffers[p_socket].m_buffer, header_message_length);
			m_buffers[p_socket].m_buffer = temp;

			//add the new message to the message queue
			short message_type;
			memcpy(&message_type, message+sizeof(char)*5+sizeof(short), sizeof(short));
			Data d = Data(message_type, message, message_length);
			m_messages.push(d);
		}
	}
}


void Network::sendData(Data &p_data)
{
	header *h = new header;
	memcpy(h->id, "TWUG", sizeof(char)*5);
	h->version = NETWORK_VERSION;
	h->message_type = p_data.getType();
	h->message_length = p_data.getLength();

	m_socket.send((void*)h, sizeof(header));
	m_socket.send(p_data.getData(), p_data.getLength());
}
bool Network::getData(Data &p_data)
{
	if(!m_messages.empty())
	{		Data d = m_messages.front();
		m_messages.pop();
		return true;
	}
	else
		return false;
}
