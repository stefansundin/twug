#include "DataKeeper.h"

DataKeeper::DataKeeper()
{
	m_sending = false;
}

void DataKeeper::lock()
{
	m_lock.lock();
}

void DataKeeper::unlock()
{
	m_lock.unlock();
}

void DataKeeper::setSending(bool p_sending)
{
	lock();
	m_sending = p_sending;
	unlock();
}

bool DataKeeper::getSending()
{
	lock();
	bool temp = m_sending;
	unlock();
	return temp;
}

