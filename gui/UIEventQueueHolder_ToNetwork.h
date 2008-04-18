#ifndef UIEventQueueHolder_ToNetwork_h
#define UIEventQueueHolder_ToNetwork_h

#include "../UIEventQueueHolder.h"
#include <gtkmm/main.h>

class UIEventQueueHolder_ToNetwork : public UIEventQueueHolder
{
public:
	UIEventQueueHolder_ToNetwork(UIEventQueue* p_eventqueue);
	void pushEvent(UIEvent p_event);
protected:
	Glib::RefPtr<Glib::IOChannel> m_iochannel;
};

#endif

