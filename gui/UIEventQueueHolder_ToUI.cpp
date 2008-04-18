#include "UIEventQueueHolder_ToUI.h"

UIEventQueueHolder_ToUI::UIEventQueueHolder_ToUI(UIEventQueue* p_eventqueue, void (*p_funptr)() ) : UIEventQueueHolder(p_eventqueue)
{
	m_funptr = p_funptr;
}

void UIEventQueueHolder_ToUI::pushEvent(UIEvent p_event)
{
	m_eventqueue->pushEvent(p_event);
	m_funptr();
}

int UIEventQueueHolder_ToUI::getReadFd() const
{
	return m_eventqueue->getReadFd();
}	
