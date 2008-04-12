#include "UIEventQueue.h"


void UIEventQueue::lock()
{
	print_me("###LOCKING "+m_filepath);
	/*bool completed=0;
	while(!completed)
	{
		if(!m_lock)
		{
			m_lock = true;
			completed = true;
		}
	}*/
	pthread_mutex_lock(&m_mutex);
	print_me("###GOT LOCK!");
}
void UIEventQueue::unlock()
{
	print_me("###UNLOCKING "+m_filepath);
	//m_lock = false;
	pthread_mutex_unlock(&m_mutex);
	print_me("###UNLOCKED");
}

UIEventQueue::UIEventQueue(std::string p_name)
{
	pthread_mutex_init (&m_mutex, NULL);

	m_name = p_name;
	m_filepath = p_name + ".temp";
	//m_lock = 0;

 	if (access(m_filepath.c_str(), F_OK) == -1)
	{
		if (mkfifo(m_filepath.c_str(), 0666) != 0)
		{
			print_me("error creating fifo");
		}
	}
}
std::string UIEventQueue::getFilePath() const
{
	return m_filepath;
}
std::string UIEventQueue::getName() const
{
	return m_name;
}	
UIEvent UIEventQueue::popEvent()
{
	print_me("UIEventQueue("+m_name+"): Popping event");
	UIEvent event = UIEvent("EMPTY");
	lock();
	if(!m_queue.empty())
	{
		event = m_queue.front();
		m_queue.pop();
	}			
	unlock();
	print_me("UIEventQueue("+m_name+"): Popped event");
	return event;
}
void UIEventQueue::pushEvent(UIEvent p_event)
{
	print_me("UIEventQueue("+m_name+"): Pushing event");
	lock();

	m_queue.push(p_event);

	unlock();
	print_me("UIEventQueue("+m_name+"): Pushed event");
}
