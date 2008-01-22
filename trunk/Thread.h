class Thread {
public:
	
private:
	Thread();
	~Thread();
};

// denna klass är det som ska köras i en separat tråd. den ska bl.a. lyssna på socket, sköta ljud, osv
// till detta har den självklart många andra klasser till hjälp, som till exempel AudioCore, som sköter ljud I/O, vår Socket-klass, osv
// den måste även kommunicera med Handler, som körs i UI:ets tråd
