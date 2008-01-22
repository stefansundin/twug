//denna klass ska instanceras av antingen MFC eller GTKMM UI:et, och sköter kommunikation med programmets extratråd

class Handler {
public:
	Handler(); //startar en tråd som ska köra en instans av klassen "Thread"
	
	~Handler();
	vector<string> getChannelMembers(string channel_name);
	//returnerar en vector med personerna i den specifierade kanalen

	vector<string> getChannels();
	//returnerar alla channels, inkl de som är utanför någon channel, dvs "lobbyn"

	bool joinChannel(string channel_name)
	//försöker joina channeln channel_name och returnar true om det går annars false

	bool connectToServer(string ip)
	//försöker ansluta till servern "ip", passa NULL så försöker den disconnecta bara.
	//returnerar true om det lyckas...

	void iStartTalking();
	//när du trycker ner push-to-talk knappen så körs denna i början 

	void iStopTalking();
	//körs när du släpper push-to-talk
private:
	// whatever
};
