LOCAL_OBJECTS = UIEventQueue.o UIEventQueueHolder.o UIEvent.o Lock.o UIManager.o NetworkManager.o main.o

CFLAGS = -g

all: client server

install: install_server install_client

install_client:
	cp twug /usr/bin
	cp gui/twug.png /usr/share/pixmaps
	cp gui/twug.desktop /usr/share/applications

install_server:
	cp server /usr/bin/twug-server

uninstall: uninstall_server uninstall_client

uninstall_client:
	rm -f /usr/bin/twug
	rm -f /usr/share/pixmaps/twug.png
	rm -f /usr/share/applications/twug.desktop

uninstall_server:
	rm -f /usr/bin/twug-server

make_network:
	cd network/; make client_objects

make_gui:
	cd gui/; make

server:
	cd network/; make server
	cp network/server .

client: make_gui make_network $(LOCAL_OBJECTS)
	@echo "#\n# Compiling and linking with network and gui\n#"
	g++ $(CFLAGS) -o twug `pkg-config gtkmm-2.4 --cflags --libs` network/ClientPool.o network/Channel.o network/Client.o network/ClientNetwork.o  network/Network.o network/Buffer.o network/Data.o network/Message.o network/debug.o network/strip.o network/fill.o gui/MainWindow.o  gui/TrayIcon.o gui/PrefsWindow.o gui/MessageHandler.o gui/MessageWindow.o gui/UIEvents.o gui/UIEventQueueHolder_ToUI.o gui/UIEventQueueHolder_ToNetwork.o gui/ClientSettings.o gui/ChannelList.o /usr/lib/libportaudio.so.2 $(LOCAL_OBJECTS)
	
win32-client: make_gui make_network $(LOCAL_OBJECTS)
	@echo "#\n# Compiling and linking with network and gui\n#"
	g++ $(CFLAGS) -o twug.exe network/ClientPool.o network/Channel.o network/Client.o network/ClientNetwork.o  network/Network.o network/Buffer.o network/Data.o network/Message.o network/debug.o network/strip.o network/fill.o gui/MainWindow.o  gui/TrayIcon.o gui/PrefsWindow.o gui/MessageHandler.o gui/MessageWindow.o gui/UIEvents.o gui/UIEventQueueHolder_ToUI.o gui/UIEventQueueHolder_ToNetwork.o gui/ClientSettings.o gui/ChannelList.o $(LOCAL_OBJECTS) -mwindows -mconsole  `pkg-config gtkmm-2.4 --libs` -L/home/r/portaudio/lib/.libs -lportaudio -lws2_32 

.cpp.o :
	gcc $(CFLAGS) `pkg-config gtkmm-2.4 --cflags` -c -o $@ $<

clean :
	@echo "#\n# Cleaning up\n#"
	-rm *.o server twug
	cd network/; make clean
	cd gui/; make clean

