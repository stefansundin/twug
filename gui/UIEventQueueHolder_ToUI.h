#ifndef UIEventQueueHolder_ToUI_h
#define UIEventQueueHolder_ToUI_h

#include "../UIEventQueueHolder.h"

class UIEventQueueHolder_ToUI : public UIEventQueueHolder
{
public:
	UIEventQueueHolder_ToUI(UIEventQueue* p_eventqueue, void (*p_funptr)() );
	void pushEvent(UIEvent p_event);
	int getReadFd() const;	
private:
	void (*m_funptr)();
};

#endif


