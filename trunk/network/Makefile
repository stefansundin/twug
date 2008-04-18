OLD_OBJECTS = Channel.o ChannelList.o Client.o ClientNetwork.o Data.o Network.o ServerNetwork.o strip.o fill.o

COMMON_OBJECTS = Network.o Data.o strip.o fill.o ClientPool.o Buffer.o Message.o
SERVER_OBJECTS = ServerNetwork.o server.o
CLIENT_OBJECTS = ClientNetwork.o client.o

CFLAGS = -g -O0

all : client_objects server

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
	@echo "#\n# Cleaning network\n#"
	-rm *.o client server

