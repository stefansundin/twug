#include "ServerNetwork.h"

ServerNetwork::ServerNetwork()
{
	m_accepting_socket = socket(AF_INET, SOCK_STREAM, 0);
}
ServerNetwork::~ServerNetwork()
{
}

int ServerNetwork::initSocket(std::string p_bind_address, unsigned int p_bind_port)
{
	struct sockaddr_in addr;
	if( p_bind_address == "" )
	{
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		inet_pton(AF_INET, p_bind_address.c_str(), &addr.sin_addr);
	}
	addr.sin_port = htons(p_bind_port);
	int result = bind(m_accepting_socket, (struct sockaddr*)&addr, sizeof(addr));
	printf("listen returned: %d\n", listen(m_accepting_socket, 5));
	return result;
}

void ServerNetwork::processNetworking()
{
	fd_set readable;
	struct timeval tv;

	FD_ZERO(&readable);

	//add the accepting socket, which initially is the "highest" socket
	FD_SET(m_accepting_socket, &readable);
	int highest = m_accepting_socket;

	std::map<int, buffer_t>::iterator mitr;
	for(mitr = m_buffers.begin(); mitr != m_buffers.end(); mitr++)
	{
		FD_SET(mitr->first, &readable);
		if(mitr->first > highest)
			highest = mitr->first;
	}

	//select client sockets and the accepting socket
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	int select_returned = select(highest+1, &readable, NULL, NULL, &tv);
	printf("select returned: %d\n", select_returned);
	if(select_returned == -1)
	{
		report_error(strerror(errno));
	}
	else if(select_returned == 0)	//nothing selected so no point in continuing
	{
		return;
	}

	printf("highest: %d\n", highest);

	//do something about it
	int i;
	for(i = 0; i <= highest; i++)
	{
		if(FD_ISSET(i, &readable))
		{
			printf("FD_ISSET %d\n", i);

			if(i == m_accepting_socket)
			{
				printf("accepting new connection\n");

				int new_client_socket = accept(m_accepting_socket, NULL, NULL);
				if(new_client_socket == -1)
				{
					printf("errno when accepting: %s\n", strerror(errno));
				}

				buffer_t b;
				b.m_buffer = NULL;
				b.m_length = 0;
				m_buffers[new_client_socket] = b;
			}
			else	//this is a client sending us data, update its buffer
			{
				printf("updating buffer\n");
				if(!updateBuffer(i))
				{
					printf("panda bear\n");
					disconnectClient(i);
				}
			}
		}
	}
}

void ServerNetwork::disconnectClient(int p_socket)
{
	int removed = m_buffers.erase(p_socket);			//remove the client from the list of sockets
	printf("removed %d\n", removed);
	while(true)
		removed++;
	shutdown(p_socket, SHUT_RDWR);		//we dont care if this fails since (AFAIK) it only does if the socket is already disconnect (or if it's not a socket, which it should be :P)
}









