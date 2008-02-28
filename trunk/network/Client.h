#include <string>

class Client
{
public:
	Client();
	~Client();

	int getSocket();
	std::string getUsername();
	unsigned char getPrivileges();

	void setSocket(int p_socket);
	void setUsername(std::string p_username);
	void setPrivileges(unsigned char p_privileges);

private:
	int m_socket;
	std::string m_username;
	unsigned char m_privileges;		//bit #1: can talk; #2: can listen; #3: can text; #4: can get text;
};

