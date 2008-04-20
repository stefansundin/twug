#ifndef Client_h
#define Client_h

#include <string>

#include "privileges.h"

class Client
{
public:
	Client();
	Client(std::string p_name, int p_socket);
	Client(std::string p_name, int p_socket, int p_privileges);
	~Client();

	std::string getName() const;
	int getSocket() const;
	int getPrivileges() const;

private:
	std::string m_name;
	std::string m_channel_name;
	int m_socket;
	int m_privileges;
};

#endif //Client_h

