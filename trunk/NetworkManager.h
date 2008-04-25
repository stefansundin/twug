#ifndef NetworkManager_h
#define NetworkManager_h

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <cstdio>
#include <iostream>
#include "UIEventQueueHolder.h"

#include <vector>
#include <string>
#include <cstdlib>

#include "debug.h"
#include "network/ClientNetwork.h"
#include "network/ClientPool.h"

#include "portaudio.h"

#define STANDARD_PORT 6789

#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (1024)
#define NUM_CHANNELS    (1)

#if 0
	#define PA_SAMPLE_TYPE  paFloat32
	typedef float SAMPLE;
	#define SAMPLE_SILENCE  (0.0f)
#elif 0
	#define PA_SAMPLE_TYPE  paInt16
	typedef short SAMPLE;
	#define SAMPLE_SILENCE  (0)
#elif 0
	#define PA_SAMPLE_TYPE  paInt8
	typedef char SAMPLE;
	#define SAMPLE_SILENCE  (0)
#else
	#define PA_SAMPLE_TYPE  paUInt8
	typedef unsigned char SAMPLE;
	#define SAMPLE_SILENCE  (128)
#endif

struct paBuffer
{
    SAMPLE *samples;
    int frameIndex;
    int maxFrameIndex;
    int tx_pos;
    int size;
    int go;
};

struct paData
{
	paBuffer record_buffer;
    paBuffer *play_buffers;
    int num_play_buffers;
};

class EventsToUI : public UIEventQueueHolder
{
public:
	EventsToUI(UIEventQueue* p_eventqueue) : UIEventQueueHolder(p_eventqueue)
	{
		m_fd = m_eventqueue->getWriteFd();
	}
	void pushEvent(UIEvent p_event)
	{
		m_eventqueue->pushEvent(p_event);
		
		char kaka = '\n';
		write(m_fd, (void*)&kaka, 1);
	}
private:
	int m_fd;
};

class NetworkManager
{
public:
	NetworkManager(UIEventQueue* p_to_ui, UIEventQueue* p_to_network);
	~NetworkManager();

	void run(); //main loop

	void processUIEvents();
	void processNetworkEvents();

private:
	void handleNetworkMessage(Message p_message);
	void channelListChanged();
	void joinChannel(std::string p_channel_name);
	void connectToServer(std::string p_address, std::string p_username, std::string p_password);

	void disconnect();
	
	static int audioCallback(const void *inputBuffer,
		void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData);

	int m_readfd;
	int m_socket;
	EventsToUI* m_events;
	UIEventQueue* m_to_network;
	bool m_sending;

	std::string m_connected_to;
	std::string m_last_requested_nick;
	std::string m_last_requested_server;

	ClientPool m_client_pool;
	ClientNetwork m_client_network;
	
	PaStreamParameters m_streamParameters;
	PaStream* m_stream;
	paData m_data;
};

#endif //NetworkManager_h

