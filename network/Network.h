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
#include <fcntl.h>

#define NETWORK_VERSION 0

class Buffer
{
public:
	Buffer() { m_length = 0;}
	~Buffer() { m_buffer = NULL; delete m_buffer; }

	char* getBuffer() { return m_buffer; }
	int getLength() { return m_length; }

	void setBuffer(char *p_buffer) { m_buffer = p_buffer; }
	void setLength(int p_length) { m_length = p_length; }
private:
	char *m_buffer;
	int m_length;
};

class Message
{
public:
	Message() {}
	~Message() { m_data = NULL; delete m_data; }

	int getSocket() { return m_socket; }
	Data& getData() { return *m_data; }

	void setSocket(int p_socket) { m_socket = p_socket; }
	void setData(Data *p_data) { m_data = p_data; }
private:
	int m_socket;
	Data *m_data;
};

class Network
{
public:
	Network();
	virtual ~Network();

	bool getMessage(Message &p_msg);
	void sendData(int p_socket, Data &p_data);

	void setLameCallbacks(int (*p_lame_recv)(char*, unsigned int), int (*p_lame_send)(const char*, unsigned int))
	{
		m_lame_recv = p_lame_recv;
		m_lame_send = p_lame_send;
	}

protected:
	bool updateBuffer(int p_socket);		//returns false if recv() returned 0, ie the socket is shutdown on the other end

	std::queue<Message> m_messages;		//<sender_socket, message>
	std::map<int, Buffer> m_buffers;		//<socket, its_buffer>

	int (*m_lame_recv)(char*, unsigned int);		//this is lame, totally lame
	int (*m_lame_send)(const char*, unsigned int);
};

#endif //NETWORK_H_

