twug: AppMan.o Handler.o MainWindow.o TrayIcon.o PrefsWindow.o MessageHandler.o MessageWindow.o main.o
	g++ -o build/twug build/AppMan.o build/Handler.o build/MainWindow.o build/PrefsWindow.o build/TrayIcon.o build/MessageHandler.o build/MessageWindow.o build/main.o `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags --libs`

AppMan.o: AppMan.cpp AppMan.h
	g++ -c -o build/AppMan.o AppMan.cpp `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags`

Handler.o: ../handler/Handler.cpp ../handler/Handler.h
	g++ -c -o build/Handler.o ../handler/Handler.cpp `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags`

MainWindow.o: MainWindow.cpp MainWindow.h
	g++ -c -o build/MainWindow.o MainWindow.cpp `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags`

TrayIcon.o: TrayIcon.cpp TrayIcon.h
	g++ -c -o build/TrayIcon.o TrayIcon.cpp `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags`

PrefsWindow.o: PrefsWindow.cpp PrefsWindow.h
	g++ -c -o build/PrefsWindow.o PrefsWindow.cpp `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags`

MessageWindow.o: MessageWindow.cpp MessageWindow.h
	g++ -c -o build/MessageWindow.o MessageWindow.cpp `pkg-config gtkmm-2.4 --cflags`

MessageHandler.o: MessageHandler.cpp MessageHandler.h
	g++ -c -o build/MessageHandler.o MessageHandler.cpp `pkg-config gtkmm-2.4 --cflags`

main.o: main.cpp
	g++ -c -o build/main.o main.cpp `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags`





