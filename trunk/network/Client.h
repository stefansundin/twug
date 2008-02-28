#ifndef CLIENT_H_
#define CLIENT_H_

#include <string>

#include "Channel.h"

class Channel;

class Client
{
public:
	Client();
	Client(std::string p_username);
	~Client();

	Channel* getChannel();
	int getSocket();
	std::string getUsername();
	unsigned char getPrivileges();

	void setChannel(Channel *p_channel);
	void setUsername(std::string p_username);
	void setPrivileges(unsigned char p_privileges);

//the ChannelList keeps track of this
//	void setSocket(int p_socket);

private:
	Channel *m_channel;
	std::string m_username;
	unsigned char m_privileges;		//bit #1: can talk; #2: can listen; #3: can text; #4: can get text;
};

#endif //CLIENT_H_
