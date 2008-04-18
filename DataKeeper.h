#ifndef DataKeeper_h
#define DataKeeper_h

//class for keeping data shared between network and audio threads ( audio data )

#include "Lock.h"

class DataKeeper {
private:
	Lock m_lock;
public:
	DataKeeper()
	{
	}
	void lock()
	{
		m_lock.lock();
	}
	void unlock()
	{
		m_lock.unlock();
	}
};

#endif
