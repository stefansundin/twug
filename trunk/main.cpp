#include "UIEvents.h"
#include "UIManager.h"
#include "NetworkManager.h"

UIEvents* g_events;
void *network_code(void *ptr)
{
	std::cout << "Network thread started" << std::endl;
	
	//UIEvents* events = (UIEvents*) ptr;
	NetworkManager* network = new NetworkManager(g_events);
}

int main (int argc, char *argv[])
{
	//UIEvents* events = new UIEvents();
	g_events = new UIEvents();

	pthread_t thread;
	pthread_create( &thread, NULL, network_code, (void*)g_events);
	
	Gtk::Main kit(argc, argv);
	UIManager ui = UIManager(g_events);
	kit.run();

	return 0;
}

