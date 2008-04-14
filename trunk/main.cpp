#include "UIEvents.h"
#include "UIManager.h"
#include "NetworkManager.h"
#include <pthread.h>

UIManager* g_ui;

UIEventQueue* to_ui;
UIEventQueue* to_network;

void update_UItoUI ()
{
	got_here();
	g_ui->processEvents();
}


void *network_code(void *ptr)
{
	std::cout << "Network thread started" << std::endl;
	
	//UIEventsNetwork* events = (UIEventsNetwork*) ptr;
	NetworkManager* network = new NetworkManager(to_ui, to_network); // opens to_network for reading, then to_ui for writing
}

int main (int argc, char *argv[])
{
	//set up event queues
	to_ui = new UIEventQueue("to_ui");
 	to_network = new UIEventQueue("to_network");

	got_here();

	//initialize network
	pthread_t thread;
	pthread_create( &thread, NULL, network_code, NULL );

	got_here();	

	//initialize gui
	UIEvents* events = new UIEvents(to_ui, to_network, &update_UItoUI); // opens to_network for writing
	got_here();
	Gtk::Main kit(argc, argv);
	g_ui = new UIManager(events);  // opens to_ui for reading

	g_ui->trigger();
	
	got_here();

	kit.run();
	got_here();
	return 0;
}

