all: client server

make_network:
	cd network/; make

make_gui:
	cd gui/; make

server: make_network
	cp network/build/server .

client: make_gui make_network
	g++ -g -o twug `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags --libs` network/build/ClientPool.o network/build/ClientNetwork.o  network/build/Network.o network/build/Data.o network/build/debug.o network/build/strip.o network/build/fill.o gui/build/MainWindow.o  gui/build/TrayIcon.o gui/build/PrefsWindow.o gui/build/MessageHandler.o gui/build/MessageWindow.o UIEventQueue.cpp UIManager.cpp NetworkManager.cpp main.cpp



