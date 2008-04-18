#ifndef UIEvents_h
#define UIEvents_h

#include "UIEventQueueHolder_ToNetwork.h"
#include "UIEventQueueHolder_ToUI.h"

class UIEvents {
public:
	UIEvents(UIEventQueue* p_eventqueue_0, UIEventQueue* p_eventqueue_1, void (*p_funptr)());
	UIEventQueueHolder_ToUI* to_ui; 
	UIEventQueueHolder_ToNetwork* to_network;
};

#endif
