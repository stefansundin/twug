#ifndef DataKeeper_h
#define DataKeeper_h

//class for keeping data shared between network and audio threads ( audio data )

#include "Lock.h"

class DataKeeper {
private:
	bool m_sending; // whether we are should record/send audio data
	Lock m_lock;
public:
	DataKeeper();

	bool getSending();
	void setSending(bool p_sending);
};

#endif
