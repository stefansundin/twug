#ifndef NETWORK_H_
#define NETWORK_H_

#include <string>
#include <queue>

#include "header.h"
#include "message_types.h"
#include "fill.h"
#include "strip.h"
#include "Data.h"
#include "Socket.h"

#define NETWORK_VERSION 0

struct buffer
{
	char *m_buffer;
	int m_length;
};

class Network
{
public:
	Network();
	virtual ~Network();

	/* the client can subclass this class and add these functions to that subclass instead
	int connect(std::string p_address, int p_port);
	int disconnect();
	void loginRequest(std::string p_user, std::string p_password);
	void logout();
	*/

	bool getMessage(Data &p_data);
	void updateBuffer(int p_socket);

protected:
	void sendData(Data &p_data);
	bool getData(Data &p_data);

	std::queue<Data> m_messages;
	std::map<int, buffer> m_buffers;		//<socket, its_buffer>
};

#endif /*NETWORK_H_*/

