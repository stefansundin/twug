#ifndef NETWORK_H_
#define NETWORK_H_

#include <errno.h>
#include <map>
#include <string>
#include <queue>

#ifdef __linux__
#include <sys/socket.h>		//for connect, recv, send, CONSTANTS etc.
//#include <unistd.h>		//dunno why this is here
#include <arpa/inet.h>		//for htons() (atleast, maybe more :P)
#endif

#include "../debug.h"

#include "header.h"
#include "message_types.h"
#include "fill.h"
#include "strip.h"
#include "Data.h"

#define NETWORK_VERSION 0

struct buffer_t
{
	char *m_buffer;
	int m_length;
};

struct message_t
{
	int m_socket;
	Data m_data;
};

class Network
{
public:
	Network();
	virtual ~Network();

	bool getMessage(message_t &p_msg);
	void sendData(int p_socket, Data &p_data);

protected:
	bool updateBuffer(int p_socket);		//returns false if recv() returned 0, ie the socket is shutdown on the other end



	std::queue<message_t> m_messages;		//<sender_socket, message>
	std::map<int, buffer_t> m_buffers;		//<socket, its_buffer>
};

#endif /*NETWORK_H_*/

