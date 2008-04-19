#ifndef Network_h
#define Network_h

#include <errno.h>
#include <map>
#include <string>
#include <queue>

#ifdef __linux__
#include <sys/socket.h>		//for connect, recv, send, CONSTANTS etc.
//#include <unistd.h>		//dunno why this is here
#include <arpa/inet.h>		//for htons() (atleast, maybe more :P)
#endif

#include <fcntl.h>
#include <stdio.h>

#include "../debug.h"

#include "header.h"
#include "message_types.h"
#include "fill.h"
#include "strip.h"
#include "Buffer.h"
#include "Data.h"
#include "Message.h"

#define NETWORK_VERSION 0
#define HEADER_SIZE sizeof(header) //replace this with crossplatform size

class Network
{
public:
	Network();
	virtual ~Network();

	bool getMessage(Message &p_msg);
	void sendData(int p_socket, Data &p_data);

protected:
	bool updateBuffer(int p_socket);		//returns false if recv() returned 0, ie the socket is shutdown on the other end

	std::queue<Message> m_messages;		//<sender_socket, message>
	std::map<int, Buffer> m_buffers;		//<socket, its_buffer>
};

#endif //Network_h

