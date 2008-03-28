#ifndef Handler_h
#define	Handler_h

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>

#include "ClientNetwork.h"
#include "ClientPool.h"

class Handler
{
public:
	Handler(
	void (*p_cb_got_text_message)(std::string,std::string),
	void (*p_cb_error_connecting)(std::string),
	void (*p_cb_connected_to_server)(std::string, std::string),
	void (*p_cb_connection_lost)(std::string),
	void (*p_cb_channel_list_changed)(),
	int (*p_cb_read_from_socket)(char*, unsigned int)
	);
	
	~Handler();
	std::vector<std::string> getChannelMembers(std::string p_channel_name);
	//returnerar en std::vector med personerna i den specifierade kanalen

	std::vector<std::string> getChannels();
	//returnerar alla channels (inkl. __lobby__)

	void joinChannel(std::string p_channel_name);
	//försöker joina channeln channel_name

	void connectToServer(std::string p_address, std::string p_username, std::string p_password);
	//försöker ansluta till servern "p_address", som användaren "p_username" med password "p_password"

	void disconnect();
	// när funktionen körts klart så ska den vara disconnected
	// callbacken m_cb_connection_lost bör inte användas här

	void iStartTalking();
	//när du trycker ner push-to-talk knappen så körs denna i början 

	void iStopTalking();
	//körs när du släpper push-to-talk

	void sendText(std::string p_to_username, std::string p_message);
	//skicka textmeddelande till användaren "p_to_username" med innehållet "p_message" 

	int getSocket();
	// returns the socket file descriptor. the UI will monitor it, and call the method update() when its changed

	void update();
	// described in above comment	 
private:
	void handleMessage(Message p_message);

	ClientPool m_client_pool;
	ClientNetwork m_client_network;
	
	std::string m_mynick;
	std::string m_mychannel;

	// Callback pointers
	void (*m_cb_got_text_message)(std::string, std::string);  // sender, contents
	void (*m_cb_error_connecting)(std::string); // if the error sent is ERR_IS_PASSWORD, the UI will spawn a password entry dialog, and reconnect with the specified password, if not, err is supposed to be a message to be shown in an error dialog, for instance "Lookup failed", "Connection refused", or "Username busy".. 
	void (*m_cb_connected_to_server)(std::string, std::string); // server, username
	void (*m_cb_connection_lost)(std::string); // connection unexpetedly lost, argument: error message (e.g. Connection refused)
	void (*m_cb_channel_list_changed)();
	int (*m_cb_read_from_socket)(char*, unsigned int);
};

#endif //Handler_h

