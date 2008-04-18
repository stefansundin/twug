OBJECTS = MainWindow.o MessageHandler.o MessageWindow.o TrayIcon.o PrefsWindow.o MessageWindow.o

CFLAGS = -g `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags`

all: $(OBJECTS)

#this is not used anymore

MainWindow: MainWindow.cpp MainWindow.h MessageHandler.cpp MessageHandler.h MessageWindow.h MessageWindow.cpp
	g++ -g -c -o build/MainWindow.o MainWindow.cpp `pkg-config gtkmm-2.4 --cflags`

TrayIcon: TrayIcon.cpp TrayIcon.h
	g++ -g -c -o build/TrayIcon.o TrayIcon.cpp `pkg-config gtkmm-2.4 --cflags`

PrefsWindow: PrefsWindow.cpp PrefsWindow.h
	g++ -g -c -o build/PrefsWindow.o PrefsWindow.cpp `pkg-config gtkmm-2.4 gconfmm-2.6 --cflags`

MessageWindow: MessageWindow.cpp MessageWindow.h
	g++ -g -c -o build/MessageWindow.o MessageWindow.cpp `pkg-config gtkmm-2.4 --cflags`

MessageHandler: MessageHandler.cpp MessageHandler.h MessageWindow.h MessageWindow.cpp
	g++ -g -c -o build/MessageHandler.o MessageHandler.cpp `pkg-config gtkmm-2.4 --cflags`

.cpp.o :
	gcc $(CFLAGS) -c -fPIC -o $@ $<

clean :
	-rm *.o

