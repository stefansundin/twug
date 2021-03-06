#include "UIEventQueueHolder_ToNetwork.h"

UIEventQueueHolder_ToNetwork::UIEventQueueHolder_ToNetwork(UIEventQueue* p_eventqueue) : UIEventQueueHolder(p_eventqueue)
{
#ifndef _WIN32
	m_iochannel = Glib::IOChannel::create_from_fd(m_eventqueue->getWriteFd());		
	m_iochannel->set_encoding();
	m_iochannel->set_buffered(0);
#endif
}
void UIEventQueueHolder_ToNetwork::pushEvent(UIEvent p_event)
{
	m_eventqueue->pushEvent(p_event);
#ifndef _WIN32
	m_iochannel->write("\n");
#endif	
}

