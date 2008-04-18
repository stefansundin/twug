#ifndef Lock_h
#define Lock_h

//class for keeping data shared between network and audio threads ( audio data )

#include <pthread.h>

class Lock {
private:
	pthread_mutex_t m_mutex;
public:
	Lock()
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
