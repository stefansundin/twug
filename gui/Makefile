OBJECTS = MainWindow.o MessageHandler.o MessageWindow.o TrayIcon.o PrefsWindow.o MessageWindow.o UIEventQueueHolder_ToUI.o UIEventQueueHolder_ToNetwork.o UIEvents.o ChannelList.o ClientSettings.o

CFLAGS = -g `pkg-config gtkmm-2.4 --cflags`

all: $(OBJECTS)

.cpp.o :
	gcc $(CFLAGS) -c -fPIC -o $@ $<

clean :
	@echo "#\n# Cleaning gui\n#"
	-rm *.o

