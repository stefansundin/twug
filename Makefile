LOCAL_OBJECTS = UIEventQueue.o UIManager.o AudioManager.o NetworkManager.o main.o

CFLAGS = -g

all: client server

make_network:
	cd network/; make

make_gui:
	cd gui/; make

server: make_network
	cp network/server .

client: make_gui make_network $(LOCAL_OBJECTS)
	g++ $(CFLAGS) -o twug `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags --libs` network/ClientPool.o network/ClientNetwork.o  network/Network.o network/Data.o network/debug.o network/strip.o network/fill.o gui/MainWindow.o  gui/TrayIcon.o gui/PrefsWindow.o gui/MessageHandler.o gui/MessageWindow.o $(LOCAL_OBJECTS)

.cpp.o :
	gcc $(CFLAGS) `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags` -c -o $@ $<

clean :
	-rm *.o
	cd network/; make clean
	cd gui/; make clean

