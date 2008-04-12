#include "UIEvents.h"
#include "UIManager.h"
#include "NetworkManager.h"
#include <pthread.h>

//UIEventsNetwork* g_events;

void *network_code(void *ptr)
{
	std::cout << "Network thread started" << std::endl;
	
	UIEventsNetwork* events = (UIEventsNetwork*) ptr;
	NetworkManager* network = new NetworkManager(events);
}

int main (int argc, char *argv[])
{
	//set up event queues
	UIEventQueue* to_ui = new UIEventQueue("to_ui");
	UIEventQueue* to_network = new UIEventQueue("to_network");

	//initialize network
	UIEventsNetwork* networkevents = new UIEventsNetwork(to_ui, to_network);
	pthread_t thread;
	pthread_create( &thread, NULL, network_code, (void*)networkevents);
	
	//initialize gui
	UIEvents* events = new UIEvents(to_ui, to_network);
	Gtk::Main kit(argc, argv);
	UIManager ui = UIManager(events);
	kit.run();

	return 0;
}

