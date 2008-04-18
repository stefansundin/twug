#ifndef UIEventsNetwork_h
#define UIEventsNetwork_h
#include "UIEventQueue.h"

class UIEventQueueHolder_NetworktoUI
{
public:
	UIEventQueueHolder_NetworktoUI(UIEventQueue* p_eventqueue)
	{
		got_here();
		m_eventqueue = p_eventqueue;

		got_here();
		m_f = m_eventqueue->getWriteFd();
	}
	
	void pushEvent(UIEvent p_event)
	{
		m_eventqueue->pushEvent(p_event);
		
		char kaka = '\n';
		write(m_f, (void*)&kaka, 1);
		//fflush(m_f);
	}
	UIEvent popEvent()
	{
		return m_eventqueue->popEvent();
	}
private:
	int m_f;
	UIEventQueue* m_eventqueue;	
};

class UIEventsNetwork {
public:
	UIEventsNetwork(UIEventQueue* p_eventqueue_0)
	{
		got_here();
		to_ui = new UIEventQueueHolder_NetworktoUI(p_eventqueue_0);
		got_here();
  	}
	UIEventQueueHolder_NetworktoUI* to_ui; 
};

#endif
