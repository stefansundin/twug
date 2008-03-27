#ifndef Handler_h
#define	Handler_h

#include <iostream>
#include <vector>
#include <string>

#include "ClientPool.h"

class Handler
{
public:
	Handler(
	void (*p_cb0)(std::string,std::string),
	void (*p_cb1)(std::string),
	void (*p_cb_connected_to_server)(std::string, std::string),
	void (*p_cb_connection_lost)(std::string),
	void (*p_cb_channel_list_changed)()
	);
	
	~Handler();
	std::vector<std::string> getChannelMembers(std::string p_channel_name);
	//returnerar en std::vector med personerna i den specifierade kanalen

	std::vector<std::string> getChannels();
	//returnerar alla channels (inkl. __lobby__)

	void joinChannel(std::string channel_name);
	//försöker joina channeln channel_name

	void connectToServer(std::string p_address, unsigned int p_port, std::string p_username, std::string p_password);
	//försöker ansluta till servern "p_address:p_port", som användaren "p_username" med password "p_password"

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
	ClientPool m_client_pool;
	ClientNetwork p_client_network;
	
	std::string m_mynick;
	std::string m_mychannel;

	// Callback pointers
	void (*m_cb0)(std::string, std::string);
	void (*m_cb1)(std::string);
	void (*m_cb_connected_to_server)(std::string, std::string);
	void (*m_cb_connection_lost)(std::string);
	void (*m_cb_channel_list_changed)();
};

#endif //Handler_h

