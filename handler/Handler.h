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
	void (*p_cb3)(std::string)
	);
	
	~Handler();
	std::vector<std::string> getChannelMembers(std::string channel_name);
	//returnerar en std::vector med personerna i den specifierade kanalen

	std::vector<std::string> getChannels();
	//returnerar alla channels

	void joinChannel(std::string channel_name);
	//försöker joina channeln channel_name

	void connectToServer(std::string ip, std::string nick);
	//försöker ansluta till servern "ip", som användaren "nick"

	void disconnect();
	// när funktionen körts klart så ska den vara disconnected
	// callbacken m_cb3 bör inte användas här

	void iStartTalking();
	//när du trycker ner push-to-talk knappen så körs denna i början 

	void iStopTalking();
	//körs när du släpper push-to-talk

	void postMessage(std::string destination, std::string contents);
	//skicka textmeddelande till användaren "destination" med innehållet "contents" 

	int getSocket();
	// returns the socket file descriptor. the UI will monitor it, and call the method update() when its changed

	void update();
	// described in above comment	 
private:

	std::string m_connectedTo;
	std::vector<channel> m_channels;

	// Callback pointers
	void (*m_cb0)(std::string,std::string);
	void (*m_cb1)(std::string);
	void (*m_cb2)(std::string,std::string);
	void (*m_cb3)(std::string);
};

#endif
