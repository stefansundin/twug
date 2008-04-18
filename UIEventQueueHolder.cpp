
#include "UIEventQueueHolder.h"

UIEventQueueHolder::UIEventQueueHolder(UIEventQueue* p_eventqueue)
{
	m_eventqueue = p_eventqueue;
}

UIEvent UIEventQueueHolder::popEvent()
{
	return m_eventqueue->popEvent();
}
