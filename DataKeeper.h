#ifndef DataKeeper_h
#define DataKeeper_h

//class for keeping data shared between network and audio threads ( audio data )

#include <pthread.h>

class DataKeeper {
private:
	pthread_mutex_t m_mutex;
	// keep data here
public:
	DataKeeper()
	{
		pthread_mutex_init (&m_mutex, NULL);
	}
	void lock()
	{
		pthread_mutex_lock(&m_mutex);
	}
	void unlock()
	{
		pthread_mutex_unlock(&m_mutex);
	}
};

#endif
