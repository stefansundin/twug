#include "UIEvents.h"

UIEvents::UIEvents(UIEventQueue* p_eventqueue_0, UIEventQueue* p_eventqueue_1, void (*p_funptr)())
{
	to_ui = new UIEventQueueHolder_ToUI(p_eventqueue_0, p_funptr);
	to_network = new UIEventQueueHolder_ToNetwork (p_eventqueue_1);
}
