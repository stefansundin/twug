#include "UIEvents.h"
#include "UIManager.h"
#include "NetworkManager.h"
#include "AudioManager.h"
#include <pthread.h>

#include "debug.h"

UIManager* g_ui;
DataKeeper* g_data;

UIEventQueue* g_to_ui;
UIEventQueue* g_to_network;


void update_UItoUI ()
{
	got_here();
	g_ui->processEvents();
}

void *audio_code(void *ptr)
{
	print_me("Audio thread started");

	AudioManager* audio = new AudioManager(g_data);
}


void *network_code(void *ptr)
{
	print_me("Network thread started");

	//opens to_network for reading, then to_ui for writing
	NetworkManager* network = new NetworkManager(g_to_ui, g_to_network, g_data);
	//run the network loop
	network->run();
}

int main (int argc, char *argv[])
{
	g_data = new DataKeeper(); // keeps audio data

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

	//initialize audio thread
	pthread_t audiothread;
	pthread_create( &audiothread, NULL, audio_code, NULL );

	//initialize network thread
	pthread_t networkthread;
	pthread_create( &networkthread, NULL, network_code, NULL );

	//initialize gui
	UIEvents* events = new UIEvents(g_to_ui, g_to_network, &update_UItoUI); //opens to_network for writing

	Gtk::Main kit(argc, argv);
	g_ui = new UIManager(events); //opens to_ui for reading

	g_ui->trigger();

	kit.run();
	return 0;
}

