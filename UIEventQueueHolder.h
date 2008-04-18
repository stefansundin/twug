#ifndef UIEventQueueHolder_h
#define UIEventQueueHolder_h

#include "UIEventQueue.h"

class UIEventQueueHolder
{
public:
	UIEventQueueHolder(UIEventQueue* p_eventqueue);
	UIEvent popEvent();
protected:
	UIEventQueue* m_eventqueue;
};

#endif
