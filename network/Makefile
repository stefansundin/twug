server : Data.o ServerNetwork.o Network.o strip.o fill.o server.o debug.o Client.o Channel.o
	g++ -o temp/server temp/Data.o temp/ServerNetwork.o temp/Network.o temp/strip.o temp/fill.o temp/server.o temp/debug.o temp/Client.o temp/Channel.o

client_test : Data.o ClientNetwork.o Network.o strip.o fill.o client_test.o debug.o
	g++ -o temp/client_test temp/Data.o temp/ClientNetwork.o temp/Network.o temp/strip.o temp/fill.o temp/client_test.o temp/debug.o

Client.o : Client.cpp Client.h
	g++ -c -o temp/Client.o Client.cpp

Channel.o : Channel.cpp Channel.h
	g++ -c -o temp/Channel.o Channel.cpp

Data.o : Data.cpp Data.h
	g++ -c -o temp/Data.o Data.cpp

Network.o : Network.cpp Network.h
	g++ -c -o temp/Network.o Network.cpp

ServerNetwork.o : ServerNetwork.cpp ServerNetwork.h Network.h
	g++ -c -o temp/ServerNetwork.o ServerNetwork.cpp

ClientNetwork.o : ClientNetwork.cpp ClientNetwork.h Network.h
	g++ -c -o temp/ClientNetwork.o ClientNetwork.cpp

strip.o : strip.cpp strip.h
	g++ -c -o temp/strip.o strip.cpp

fill.o : fill.cpp fill.h
	g++ -c -o temp/fill.o fill.cpp

client_test.o : client_test.cpp ClientNetwork.h
	g++ -c -o temp/client_test.o client_test.cpp

server.o : server.cpp ServerNetwork.h
	g++ -c -o temp/server.o server.cpp

debug.o : ../debug.cpp ../debug.h
	g++ -c -o temp/debug.o ../debug.cpp

clean :
	-rm temp/*.o temp/client_test temp/server_test

