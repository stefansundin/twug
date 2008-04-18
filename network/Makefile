OLD_OBJECTS = Channel.o ChannelList.o Client.o ClientNetwork.o Data.o Network.o ServerNetwork.o strip.o fill.o

COMMON_OBJECTS = Network.o Data.o strip.o fill.o ClientPool.o
SERVER_OBJECTS = ServerNetwork.o server.o
CLIENT_OBJECTS = ClientNetwork.o client.o

CFLAGS = -g -O0

all : server client_objects

server : $(COMMON_OBJECTS) $(SERVER_OBJECTS) debug
	g++ $(CFLAGS) -o server $(COMMON_OBJECTS) $(SERVER_OBJECTS) debug.o

client_objects : $(COMMON_OBJECTS) ClientNetwork.o

client : $(COMMON_OBJECTS) $(CLIENT_OBJECTS) debug
	g++ $(CFLAGS) -o client $(COMMON_OBJECTS) $(CLIENT_OBJECTS) debug.o

debug : ../debug.cpp
	g++ -c $(CFLAGS) -o debug.o ../debug.cpp

.cpp.o :
	gcc $(CFLAGS) -c -o $@ $<

clean :
	-rm $(OBJECTS) $(SERVER_OBJECTS) $(CLIENT_OBJECTS) debug.o client server

