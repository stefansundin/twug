#include "Network.h"

Network::Network()
{
	m_connected = false;
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
void Network::update()
{
	char *buffer;
	int length = m_socket.recv(buffer, 2048);

	char *temp = new char[m_buffer_length+length];
	memcpy(temp, m_buffer, m_buffer_length);
	memcpy(temp+m_buffer_length, buffer, length);

	m_buffer_length += length;
	m_buffer = temp;

	if(m_buffer_length >= sizeof(header))
	{
		int length;
		memcpy(&length, m_buffer+sizeof(char)*5+sizeof(short)*2, sizeof(int));
		if(m_buffer_length-sizeof(header) >= length)
		{
			//get the message
			int message_length = sizeof(header)+length;
			char *msg = new char[message_length];
			memcpy(msg, m_buffer, message_length);

			//remove message from m_buffer
			temp = new char[m_buffer_length-length];
			memcpy(temp, m_buffer+message_length, m_buffer_length-message_length);
			m_buffer = temp;

			short message_type;
			memcpy(&message_type, msg+sizeof(char)*5+sizeof(short), sizeof(short));
			Data d = Data(message_type, msg, message_length);
			m_message_buffer.push(d);
		}
	}
}


int Network::sendData(Data &p_data)
{
	header *h = new header;
	memcpy(h->id, "TWUG", sizeof(char)*5);
	h->version = NETWORK_VERSION;
	h->message_type = p_data.getType();
	h->message_length = p_data.getLength();

	m_socket.send((void*)h, sizeof(header));
	m_socket.send(p_data.getData(), p_data.getLength());

	return 0;
}
int Network::getData(Data &p_data)
{
	//socket.read() to a buffer
	//get a whole message from the buffer if there is one
	//process it
	if(!m_message_buffer.empty())
	{
//		the_mentioned_complete_message		Data(123, "sender", sizeof(char)*7);
	}
	else
		return 0;
}
