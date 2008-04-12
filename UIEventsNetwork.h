#ifndef UIEventsNetwork_h
#define UIEventsNetwork_h
#include "UIEventQueue.h"

class UIEventQueueHolder_other
{
public:
	UIEventQueueHolder_other(UIEventQueue* p_eventqueue)
	{
		m_eventqueue = p_eventqueue;
	}
	void pushEvent(UIEvent p_event)
	{
		m_eventqueue->pushEvent(p_event);
		
		FILE* f = fopen(m_eventqueue->getFilePath().c_str(), "w");
		fputc('\n', f);
		fclose(f);
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
	UIEventQueue* m_eventqueue;	
};

class UIEventsNetwork {
public:
	UIEventsNetwork(UIEventQueue* p_eventqueue_0, UIEventQueue* p_eventqueue_1)
	{
		to_ui = new UIEventQueueHolder_other(p_eventqueue_0);
		to_network = p_eventqueue_1;
  	}
	UIEventQueueHolder_other* to_ui; 
	UIEventQueue* to_network;
};

#endif
