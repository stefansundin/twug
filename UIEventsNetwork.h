#ifndef UIEventsNetwork_h
#define UIEventsNetwork_h


class UIEventQueueHolder_other
{
public:
	UIEventQueueHolder_other(UIEventQueue* p_eventqueue);
	void pushEvent(UIEvent p_event)
	{
		m_eventqueue->pushEvent(p_event);
		
		FILE* f = fopen(p_eventqueue->getFilePath().c_str(), "w");
		fputc('\n', f);
		fclose(f);
	}
	UIEvent popEvent();
private:
	UIEventQueue* m_eventqueue;	
};

class UIEventsForNetwork {
public:
	UIEvents(UIEventQueue* p_eventqueue)
	{
		to_ui = new UIEventQueueHolder_other(p_eventqueue);
  	}
	UIEventQueueHolder_other* to_ui; 
};

#endif
