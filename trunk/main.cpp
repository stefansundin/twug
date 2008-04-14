#include "UIEvents.h"
#include "UIManager.h"
#include "NetworkManager.h"
#include "AudioManager.h"
#include <pthread.h>

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
	std::cout << "Audio thread started" << std::endl;
	
	AudioManager* audio = new AudioManager(g_data);
}


void *network_code(void *ptr)
{
	std::cout << "Network thread started" << std::endl;
	
	NetworkManager* network = new NetworkManager(g_to_ui, g_to_network, g_data); // opens to_network for reading, then to_ui for writing
}

int main (int argc, char *argv[])
{
	g_data = new DataKeeper(); // keeps audio data

	//set up event queues
	g_to_ui = new UIEventQueue("to_ui");
 	g_to_network = new UIEventQueue("to_network");

	got_here();

	//initialize audio thread
	pthread_t audiothread;
	pthread_create( &audiothread, NULL, audio_code, NULL );

	//initialize network thread
	pthread_t networkthread;
	pthread_create( &networkthread, NULL, network_code, NULL );

	got_here();	

	//initialize gui
	UIEvents* events = new UIEvents(g_to_ui, g_to_network, &update_UItoUI); // opens to_network for writing
	got_here();
	Gtk::Main kit(argc, argv);
	g_ui = new UIManager(events);  // opens to_ui for reading

	g_ui->trigger();
	
	got_here();

	kit.run();
	got_here();
	return 0;
}

