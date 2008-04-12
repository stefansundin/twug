#ifndef UIEventQueue_h
#define UIEventQueue_h
#include "UIEvent.h"
#include <queue>
#include <fcntl.h>

class UIEventQueue {
private:
	std::queue<UIEvent> m_queue;
	std::string m_filepath;
	std::string m_name;
	void lock();
	void unlock();
	bool m_lock;
public:
	UIEventQueue(std::string p_name);
	std::string getFilePath() const;
	std::string getName() const;
	UIEvent popEvent();
	void pushEvent(UIEvent p_event);
};

#endif
