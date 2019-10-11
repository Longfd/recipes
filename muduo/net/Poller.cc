#include "Poller.h"
#include "Channel.h"

#include <iostream>
#include <stdlib.h>
#include <assert.h>


Poller::Poller(EventLoop* loop)
	:ownerLoop_(loop)
{
}

Poller::~Poller()
{
}

Poller::poll(int timeoutMs, ChannelList* activeChannels)
{
	int numEvents = poll(pollfds_.data(), pollfds_.size(), timeoutMs);
	TimeStamp now(TimeStamp::now());

	if(numEvents > 0) {
		std::cout << numEvents << "events happend\n";
		fillActiveChannels(numEvents, activeChannels);
	}
	else if(numEvents == 0) {
		std::cout << "nothing happend\n";
	}
	else {
		std::cout << "Poller::poll() error!\n";
		exit(1);
	}

	return now;
}


void Poller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
	PollFdList::const_iterator it = pollfds_.begin();
	for(; it != pollfds_.end() && numEvents > 0; ++it) 
	{
		if(it->revents > 0) 
		{
			--numEvents;
			ChannelMap::const_iterator ch = channels_.find(it->fd);
			assert(ch != channels_.end());

			Channel* channel = ch->second;
			assert(channel->fd == it->fd);
			channel->set_revents(it->revents);

			activeChannels->push_back(channel);
		}
	} // end for
}












