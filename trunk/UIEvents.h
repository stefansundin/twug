#ifndef UIEvents_h
#define UIEvents_h

#include <vector>
#include <string>
#include <queue>
#include <iostream>
#include <fcntl.h>
#include <gtkmm/main.h>
#include "UIEventsNetwork.h"

class UIEventQueueHolder_UItoNetwork
{
public:
	UIEventQueueHolder_UItoNetwork(UIEventQueue* p_eventqueue)
	{
		m_eventqueue = p_eventqueue;
		m_iochannel = Glib::IOChannel::create_from_file(m_eventqueue->getFilePath() , "w" );		
		m_iochannel->set_encoding();
		m_iochannel->set_buffered(0);
	}
	void pushEvent(UIEvent p_event)
	{
		m_eventqueue->pushEvent(p_event);
	
		m_iochannel->write("\n");
		
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
	Glib::RefPtr<Glib::IOChannel> m_iochannel;
	UIEventQueue* m_eventqueue;
};


class UIEvents {
public:
	UIEvents(UIEventQueue* p_eventqueue_0, UIEventQueue* p_eventqueue_1)
	{
		to_ui = new UIEventQueueHolder_other(p_eventqueue_0);
		to_network = new UIEventQueueHolder_UItoNetwork (p_eventqueue_1);
		
  	}
	UIEventQueueHolder_other* to_ui; 
	UIEventQueueHolder_UItoNetwork* to_network;
};


#endif
