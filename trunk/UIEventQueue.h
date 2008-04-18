#ifndef UIEventQueue_h
#define UIEventQueue_h

#include "debug.h"

#include "UIEvent.h"
#include <queue>
#include <fcntl.h>
#include "Lock.h"

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
	UIEventQueue(std::string p_name, int p_readfd, int p_writefd);
	int getReadFd() const;
	int getWriteFd() const;
	std::string getName() const;
	UIEvent popEvent();
	void pushEvent(UIEvent p_event);
};

#endif
