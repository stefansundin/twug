#include <gtkmm/main.h>
#include "AppMan.h"

int main (int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);
	AppMan app;
	kit.run();
	return 0;
}

