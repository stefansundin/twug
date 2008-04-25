#ifndef Lock_h
#define Lock_h

//class for mutex locking (needed to share data between threads)

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
