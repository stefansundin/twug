#ifndef HANDLER_H
#define	HANDLER_H

#include <iostream>
#include <vector>
#include <string>

struct channel
{
	std::string name;
	std::vector<std::string> members;
};

class Handler {
public:
	Handler(
	void (*p_cb0)(std::string,std::string),
	void (*p_cb1)(std::string),
	void (*p_cb2)(std::string,std::string),
	void (*p_cb3)(std::string),
	void (*p_cb4)(),
	int (*p_cb_read_from_socket)(char*, unsigned int),
	int (*p_cb_write_to_socket)(const char*, unsigned int)
	);
	
	~Handler();
	std::vector<std::string> getChannelMembers(std::string channel_name);
	//returnerar en std::vector med personerna i den specifierade kanalen

	std::vector<std::string> getChannels();
	//returnerar alla channels (inkl. __lobby__)

	void joinChannel(std::string channel_name);
	//försöker joina channeln channel_name

	void connectToServer(std::string ip, std::string nick, std::string p_password);
	//försöker ansluta till servern "ip", som användaren "nick"

	void disconnect();
	// när funktionen körts klart så ska den vara disconnected
	// callbacken m_cb3 bör inte användas här

	void iStartTalking();
	//när du trycker ner push-to-talk knappen så körs denna i början 

	void iStopTalking();
	//körs när du släpper push-to-talk

	void sendText(std::string destination, std::string contents);
	//skicka textmeddelande till användaren "destination" med innehållet "contents" 

	int getSocket();
	// returns the socket file descriptor. the UI will monitor it, and call the method update() when its changed

	void update();
	// described in above comment	 
private:
	std::vector<channel> m_channels;
	
	std::string m_mynick;
	std::string m_mychannel;

	// Callback pointers
	void (*m_cb0)(std::string,std::string);
	void (*m_cb1)(std::string);
	void (*m_cb2)(std::string,std::string);
	void (*m_cb3)(std::string);
	void (*m_cb4)();
};

#endif
