#ifndef UIEvents_h
#define UIEvents_h

#include <vector>
#include <string>
#include <queue>
#include <iostream>
#include <fcntl.h>

class UIEventQueueHolder_UItoNetwork
{
public:
	UIEventQueueHolder_UItoNetwork(UIEventQueue* p_eventqueue)
	{
		m_eventqueue = p_eventqueue;
		m_iochannel = Glib::IOChannel::create_from_file(m_eventqueue->getFilePath() , "w" );		
	}
	void pushEvent(UIEvent p_event);
	UIEvent popEvent();
private:
	Glib::RefPtr<Glib::IOChannel> m_iochannel;
	UIEventQueue* m_eventqueue;
};


class UIEvents {
public:
	UIEvents(p_eventqueue_0, p_eventqueue_1)
	{
		to_ui = new UIEventQueueHolder_other(p_eventqueue_0);
		to_network = new UIEventQueueHolder_UItoNetwork (p_eventqueue_1);
		
  	}
	UIEventQueueHolder_other* to_ui; 
	UIEventQueueHolder_UItoNetwork* to_network;
};


#endif
