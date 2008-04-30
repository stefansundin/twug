#include "UIManager.h"
#include "NetworkManager.h"

#include "debug.h"

UIManager* g_ui;

UIEventQueue* g_to_ui;
UIEventQueue* g_to_network;


void update_UItoUI ()
{
	got_here();
	g_ui->processEvents();
}

void *network_code(void *ptr)
{
	print_me("Network thread started");

	NetworkManager* network = new NetworkManager(g_to_ui, g_to_network);
	network->run();
}

int main (int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);

	#ifndef _WIN32 
	//set up event queues
	int to_network_pipes[2];
	int to_ui_pipes[2];
	if ( pipe( to_network_pipes ) != 0 )
	{
		print_me("Error");
		return 0;
	}
	if ( pipe( to_ui_pipes ) != 0 )
	{
		print_me("Error");
		return 0;
	}
	g_to_ui = new UIEventQueue("to_ui", to_ui_pipes[0], to_ui_pipes[1]);
 	g_to_network = new UIEventQueue("to_network", to_network_pipes[0], to_network_pipes[1]);

	//initialize network thread
	pthread_t networkthread;
	pthread_create( &networkthread, NULL, network_code, NULL );

	#else
	g_to_ui = new UIEventQueue("to_ui");
 	g_to_network = new UIEventQueue("to_network");

	DWORD id;
	CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE) &network_code,NULL,0,&id);
	#endif	

	//initialize gui
	g_ui = new UIManager(g_to_ui, g_to_network, &update_UItoUI);
	g_ui->trigger();

	kit.run();

	return 0;
}

