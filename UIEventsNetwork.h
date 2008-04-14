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
		m_f = fopen(m_eventqueue->getFilePath().c_str(), "w");
	}
	
	void pushEvent(UIEvent p_event)
	{
		m_eventqueue->pushEvent(p_event);
		
		fputc('\n', m_f);
		fflush(m_f);
	}
	UIEvent popEvent()
	{
		return m_eventqueue->popEvent();
	}
	std::string getFilePath() const
	{
		return m_eventqueue->getFilePath();
	}
private:
	FILE* m_f;
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
