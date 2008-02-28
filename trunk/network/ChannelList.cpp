#include "ChannelList.h"

ChannelList::ChannelList()
{
	newChannel(LOBBY);
}
ChannelList::~ChannelList()
{
}


bool ChannelList::newChannel(std::string p_name)
{
	for(int i = 0; i < m_channels.size(); i++)
	{
		if(m_channels.at(i)->getName() == p_name)
			return false;
	}
	Channel *c = new Channel(p_name);
	m_channels.push_back(c);
	return true;
}
bool ChannelList::removeChannel(std::string p_name)
{
	if(p_name == LOBBY);
		return false;

	std::vector<Channel*>::iterator citr;
	for(citr = m_channels.begin(); citr != m_channels.end(); citr++)
	{
		if((*citr)->getName() == p_name)
		{
			m_channels.erase(citr);
			return true;
		}
	}
	return false;
}


bool ChannelList::newClient(int p_socket, std::string p_name)
{
		try
		{
	std::map<int, Client*>::iterator citr;
	for(citr = m_clients.begin(); citr != m_clients.end(); citr++)
	{
		if((*citr).second->getUsername() == p_name)
			return false;
	}
		}
		catch(...)
		{
			printf("0x0000\n");
		}

	Client *c = new Client(p_name);
	m_clients[p_socket] = c;
	changeClientChannel(p_name, LOBBY, "");		//move the new client to the lobby
}
bool ChannelList::removeClient(std::string p_name)
{
	Client *client_to_remove = NULL;
	std::map<int, Client*>::iterator citr;
	for(citr = m_clients.begin(); citr != m_clients.end(); citr++)
	{
		if((*citr).second->getUsername() == p_name)
			client_to_remove = (*citr).second;
	}
	if(client_to_remove == NULL)	//client not found
		return false;

	client_to_remove->getChannel()->removeClient(client_to_remove);		//remove client from its channel
	m_clients.erase(--citr);	//remove the client from the list of clients, -- cuz it get ++ in the for-loop one time too many
	delete client_to_remove;	//free the memory it took up
	return true;	//client removed
}
bool ChannelList::removeClient(int p_socket)
{
	return removeClient(m_clients[p_socket]->getUsername());
}


int ChannelList::changeClientChannel(std::string p_client_name, std::string p_channel_name, std::string p_password)
{
	if(p_client_name.size() > 20)		//we dont support longer names
		return -1;
	if(p_channel_name.size() > 20)		//same here
		return -2;

	Client *client_to_move = NULL;
	Channel *channel_destination = NULL;

	std::map<int, Client*>::iterator citr;

	for(citr = m_clients.begin(); citr != m_clients.end(); citr++)
	{
		if((*citr).second->getUsername() == p_client_name)
			client_to_move = (*citr).second;
	}

	if(client_to_move == NULL)			//client not found
		return -3;

	int ch;

	for(ch = 0; ch < m_channels.size(); ch++)
	{
		if(m_channels.at(ch)->getName() == p_channel_name)
			channel_destination = m_channels.at(ch);
	}

	if(channel_destination == NULL)		//channel not found
		return -4;
	else if(channel_destination->getPassword() != p_password)	//bad password
		return -5;

	//now we have both the client and the new channel

	Channel *old_channel = client_to_move->getChannel();
	if(old_channel != NULL)
		old_channel->removeClient(client_to_move);			//remove client from the old channel

	client_to_move->setChannel(channel_destination);		//update the client
	channel_destination->addClient(client_to_move);			//add the client to the new channel

	return 0; //success :D
}
int ChannelList::changeClientChannel(int p_socket, std::string p_channel_name, std::string p_password)
{
	std::string username = m_clients[p_socket]->getUsername();
	return changeClientChannel(username, p_channel_name, p_password);
}


void ChannelList::print()		//server debug thingy
{
	printf("----Channels---------------\n");

	int ch;
	for(ch = 0; ch < m_channels.size(); ch++)
	{
		m_channels.at(ch)->print(1);		//1 means 1 extra tab out
	}

	printf("---------------------------\n");
}





