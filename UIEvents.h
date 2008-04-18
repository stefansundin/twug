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
		got_here();

		m_eventqueue = p_eventqueue;
	
		got_here();

		m_iochannel = Glib::IOChannel::create_from_fd(m_eventqueue->getWriteFd());		
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
private:
	Glib::RefPtr<Glib::IOChannel> m_iochannel;
	UIEventQueue* m_eventqueue;
};

class UIEventQueueHolder_UItoUI
{
public:
	UIEventQueueHolder_UItoUI(UIEventQueue* p_eventqueue, void (*p_funptr)() )
	{
		m_funptr = p_funptr;
		got_here();
		m_eventqueue = p_eventqueue;
		got_here();
	}
	void pushEvent(UIEvent p_event)
	{
		m_eventqueue->pushEvent(p_event);

		m_funptr();
	}
	UIEvent popEvent()
	{
		return m_eventqueue->popEvent();
	}
	int getReadFd() const
	{
		return m_eventqueue->getReadFd();
	}	
private:
	void (*m_funptr)();
	UIEventQueue* m_eventqueue;
};


class UIEvents {
public:
	UIEvents(UIEventQueue* p_eventqueue_0, UIEventQueue* p_eventqueue_1, void (*p_funptr)())
	{
		got_here();
		to_ui = new UIEventQueueHolder_UItoUI(p_eventqueue_0, p_funptr);
		got_here();
		to_network = new UIEventQueueHolder_UItoNetwork (p_eventqueue_1);
		got_here();
		
  	}
	UIEventQueueHolder_UItoUI* to_ui; 
	UIEventQueueHolder_UItoNetwork* to_network;
};


#endif
