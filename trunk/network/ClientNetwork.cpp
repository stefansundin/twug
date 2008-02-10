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
		m_connected = true;
	return status;
}

int ClientNetwork::disconnect()
{
	int status = shutdown(m_socket, SHUT_RDWR);
	if(status == 0)
		m_connected = false;
	return status;
}

void ClientNetwork::loginRequest(std::string p_user, std::string p_password)
{
	fill(p_user, 20);
	fill(p_password, 20);
	std::string temp = p_user + p_password;

	printf("sent message length: %d\n", temp.size());
	printf("kaka: %d\n", strlen(temp.c_str()));
	printf("kaka: %d\n", sizeof(temp.c_str()));

	Data data = Data(CLIENT_LOGIN_REQUEST, temp.c_str(), temp.size());
	sendData(m_socket, data);
}
void ClientNetwork::logout()
{
	Data data = Data(CLIENT_LOGOUT, "", 0);
	sendData(m_socket, data);
}

bool ClientNetwork::processNetworking()
{
	if(!m_connected)
		return false;

	fd_set readable;
	struct timeval tv;

	FD_ZERO(&readable);
	FD_SET(m_socket, &readable);


	tv.tv_sec = 5;
	tv.tv_usec = 0;
	int select_returned = select(m_socket+1, &readable, NULL, NULL, &tv);
	if(select_returned == -1)
	{
		report_error(strerror(errno));
	}

	if(FD_ISSET(m_socket, &readable))
	{
		printf("updating buffer\n");
		if(!updateBuffer(m_socket))
		{
			m_connected = false;
		}
	}

	return true;
}

int ClientNetwork::getSocket()
{
	return m_socket;
}

