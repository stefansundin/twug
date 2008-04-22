#include "ClientNetwork.h"

ClientNetwork::ClientNetwork()
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);

	m_connected = false;
}
ClientNetwork::~ClientNetwork()
{
	disconnect();
}

int ClientNetwork::connect(std::string p_address, int p_port)
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	inet_pton(addr.sin_family, p_address.c_str(), &addr.sin_addr);
	addr.sin_port = htons(p_port);
	int status = ::connect(m_socket, (struct sockaddr*)&addr, sizeof(addr));
	if(status == 0)
	{
		printf("%d\n", m_socket);
		m_connected = true;
	}
	return status;
}
int ClientNetwork::disconnect()
{
	int status = shutdown(m_socket, SHUT_RDWR);
	if(status == 0)
		m_connected = false;
	return status;
}


bool ClientNetwork::loginRequest(std::string p_user, std::string p_password)
{
	if(!fill(p_user, MESSAGE_FILL) || !fill(p_password, MESSAGE_FILL))
	{
		return false;
	}
	std::string temp = p_user + p_password;

	Data data = Data(CLIENT_LOGIN_REQUEST, temp);
	sendData(m_socket, data);

	return true;
}
void ClientNetwork::logout()
{
	if(m_connected)
	{
		Data data = Data(CLIENT_LOGOUT, "");
		sendData(m_socket, data);
	}
}

void ClientNetwork::changeChannels(std::string p_channel, std::string p_password)
{
	fill(p_channel, MESSAGE_FILL);
	fill(p_password, MESSAGE_FILL);
	std::string to_send = p_channel + p_password;

	Data data = Data(CLIENT_CHANNEL_CHANGE, to_send);
	sendData(m_socket, data);
}

void ClientNetwork::sendText(std::string p_to_username, std::string p_message)
{
	fill(p_to_username, MESSAGE_FILL);
	std::string to_send = p_to_username + p_message;

	Data data = Data(CLIENT_TEXT_DATA, to_send);
	sendData(m_socket, data);
}
void ClientNetwork::sendTextBroadcast(std::string p_message)
{
	Data data = Data(CLIENT_TEXT_BROADCAST, p_message);
	sendData(m_socket, data);
}


bool ClientNetwork::processNetworking()
{
//	print_me("start of function");

	if(!m_connected)
		return false;


	fd_set readable;
	struct timeval tv;

	FD_ZERO(&readable);
	FD_SET(m_socket, &readable);

	tv.tv_sec = 0;
	tv.tv_usec = 0;
//	print_me("before select");
	int select_returned = select(m_socket+1, &readable, NULL, NULL, &tv);
//	print_me("after select");
	if(select_returned == -1)
	{
		report_error(strerror(errno));
	}

	if(FD_ISSET(m_socket, &readable))
	{

//		print_me("data on m_socket");
		if(!updateBuffer(m_socket))
		{
			m_connected = false;
			return false;
		}

	}
	else
	{
		print_me("no data on m_socket");
		return true;
	}

//	print_me("end of function");

	return true;
}

int ClientNetwork::getSocket() const
{
	return m_socket;
}
bool ClientNetwork::getConnectionStatus() const
{
	return m_connected;
}

void ClientNetwork::sendCreateChannelRequest(std::string p_name)
{
	fill(p_name, MESSAGE_FILL);

	Data data = Data(CLIENT_ADMIN_CREATE_CHANNEL, p_name);
	sendData(m_socket, data);
}

void ClientNetwork::sendRemoveChannelRequest(std::string p_name)
{
	fill(p_name, MESSAGE_FILL);

	Data data = Data(CLIENT_ADMIN_REMOVE_CHANNEL, p_name);
	sendData(m_socket, data);
}


