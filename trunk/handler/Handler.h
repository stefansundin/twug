#ifndef HANDLER_H
#define	HANDLER_H

//denna klass ska instanceras av GTKMM UI:et, och sköter bl.a. kommunikation med programmets extratråd

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
	Handler(); //startar en tråd som ska köra en instans av klassen "Thread"
	
	~Handler();
	std::vector<std::string> getChannelMembers(std::string channel_name);
	//returnerar en std::vector med personerna i den specifierade kanalen

	std::vector<std::string> getChannels();
	//returnerar alla channels, inkl de som är utanför någon channel, dvs "lobbyn"

	bool joinChannel(std::string channel_name);
	//försöker joina channeln channel_name och returnar true om det går annars false

	bool connectToServer(std::string ip);
	//försöker ansluta till servern "ip", passa NULL så försöker den disconnecta bara.
	//returnerar true om det lyckas...

	void iStartTalking();
	//när du trycker ner push-to-talk knappen så körs denna i början 

	void iStopTalking();
	//körs när du släpper push-to-talk

	void postMessage(std::string destination, std::string contents);
	//skicka textmeddelande till användaren "destination" med innehållet "contents" 
private:
	void disconnect();
	std::string m_connectedTo;
	std::vector<channel> m_channels;
};

#endif
