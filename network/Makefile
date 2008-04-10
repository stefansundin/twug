OLD_OBJECTS = Channel.o ChannelList.o Client.o ClientNetwork.o Data.o Network.o ServerNetwork.o strip.o fill.o

COMMON_OBJECTS = Network.o Data.o strip.o fill.o ClientPool.o
SERVER_OBJECTS = ServerNetwork.o server.o
CLIENT_OBJECTS = ClientNetwork.o client.o

CFLAGS = -g -O0

all : client server

server : $(COMMON_OBJECTS) $(SERVER_OBJECTS) debug
	g++ -c $(CFLAGS) -o build/server.o server.cpp
	cd build; g++ -o server $(COMMON_OBJECTS) $(SERVER_OBJECTS) debug.o

client : $(COMMON_OBJECTS) $(CLIENT_OBJECTS) debug
	g++ -c $(CFLAGS) -o build/client.o client.cpp
	cd build; g++ -o client $(COMMON_OBJECTS) $(CLIENT_OBJECTS) debug.o

debug : ../debug.cpp
	g++ -c $(CFLAGS) -o build/debug.o ../debug.cpp

$(COMMON_OBJECTS) $(SERVER_OBJECTS) $(CLIENT_OBJECTS) :
	g++ -c $(CFLAGS) -o build/$@ $(@:.o=.cpp)

clean :
	-cd build; rm $(OBJECTS) $(SERVER_OBJECTS) $(CLIENT_OBJECTS) debug.o client server

