all: make_shit twug

twug: AppMan.o MainWindow.o TrayIcon.o PrefsWindow.o MessageHandler.o MessageWindow.o main.o Handler.o
	g++ -g -o build/twug build/AppMan.o build/MainWindow.o build/PrefsWindow.o build/TrayIcon.o build/MessageHandler.o build/MessageWindow.o build/main.o build/Handler.o `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags --libs` ../network/build/ClientPool.o ../network/build/ClientNetwork.o  ../network/build/Network.o ../network/build/Data.o ../network/build/debug.o ../network/build/strip.o ../network/build/fill.o

make_shit:
	cd ../network/; make

AppMan.o: AppMan.cpp AppMan.h
	g++ -g -c -o build/AppMan.o AppMan.cpp `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags`

MainWindow.o: MainWindow.cpp MainWindow.h
	g++ -g -c -o build/MainWindow.o MainWindow.cpp `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags`

TrayIcon.o: TrayIcon.cpp TrayIcon.h
	g++ -g -c -o build/TrayIcon.o TrayIcon.cpp `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags`

PrefsWindow.o: PrefsWindow.cpp PrefsWindow.h
	g++ -g -c -o build/PrefsWindow.o PrefsWindow.cpp `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags`

MessageWindow.o: MessageWindow.cpp MessageWindow.h
	g++ -g -c -o build/MessageWindow.o MessageWindow.cpp `pkg-config gtkmm-2.4 --cflags`

MessageHandler.o: MessageHandler.cpp MessageHandler.h
	g++ -g -c -o build/MessageHandler.o MessageHandler.cpp `pkg-config gtkmm-2.4 --cflags`

Handler.o: ../network/Handler.cpp ../network/Handler.h
	g++ -g -c -o build/Handler.o ../network/Handler.cpp `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags`

main.o: main.cpp
	g++ -g -c -o build/main.o main.cpp `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags`
