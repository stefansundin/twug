#include "DataKeeper.h"

DataKeeper::DataKeeper()
{
}
void DataKeeper::lock()
{
	m_lock.lock();
}
void DataKeeper::unlock()
{
	m_lock.unlock();
}

