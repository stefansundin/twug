#ifndef Lock_h
#define Lock_h

//class for keeping data shared between network and audio threads ( audio data )

#include <pthread.h>

class Lock {
private:
	pthread_mutex_t m_mutex;
public:
	Lock();
	void lock();
	void unlock();
};

#endif
