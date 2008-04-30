#ifndef UIEventQueue_h
#define UIEventQueue_h

#include "debug.h"
#include "UIEvent.h"
#include <queue>
#include "Lock.h"

#ifndef _WIN32 
#include <fcntl.h>
#endif

class UIEventQueue {
private:
	std::queue<UIEvent> m_queue;
	std::string m_name;
	int m_readfd;
	int m_writefd;
	Lock m_lock;

	void lock();
	void unlock();
public:
	#ifndef _WIN32 
	UIEventQueue(std::string p_name, int p_readfd, int p_writefd);
	#else
	UIEventQueue(std::string p_name);
	#endif
	int getReadFd() const;
	int getWriteFd() const;
	std::string getName() const;
	UIEvent popEvent();
	void pushEvent(UIEvent p_event);
};

#endif
