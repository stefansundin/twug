#include "Lock.h"

#ifdef _WIN32 

Lock::Lock()
{
	m_mutex = CreateMutex(NULL, FALSE, NULL);

}
void Lock::lock()
{
	WaitForSingleObject(m_mutex,INFINITE);

}
void Lock::unlock()
{
	ReleaseMutex(m_mutex);
}


#else

Lock::Lock()
{
	pthread_mutex_init (&m_mutex, NULL);
}
void Lock::lock()
{
	pthread_mutex_lock(&m_mutex);
}
void Lock::unlock()
{
	pthread_mutex_unlock(&m_mutex);
}

#endif
