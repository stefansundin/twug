#include "Lock.h"

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
