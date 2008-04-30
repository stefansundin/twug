#ifndef Lock_h
#define Lock_h

//class for mutex locking (needed to share data between threads)

#ifdef _WIN32 
#include <windows.h>
#else
#include <pthread.h>
#endif

class Lock {
private:
	#ifdef _WIN32 
	HANDLE m_mutex;
	#else
	pthread_mutex_t m_mutex;
	#endif
public:
	Lock();
	void lock();
	void unlock();
};

#endif //Lock_h
