#include "DataKeeper.h"

DataKeeper::DataKeeper()
{
	m_sending = false;
}

void DataKeeper::setSending(bool p_sending)
{
	m_lock.lock();
	m_sending = p_sending;
	m_lock.unlock();
}

bool DataKeeper::getSending()
{
	m_lock.lock();
	bool temp = m_sending;
	m_lock.unlock();
	return temp;
}

