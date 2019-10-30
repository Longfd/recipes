#include "Poller.h"
#include "Channel.h"
#include "../base/TimeStamp.h"

#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <poll.h>
#include <algorithm>


Poller::Poller(EventLoop* loop)
	:ownerLoop_(loop)
{
}

Poller::~Poller()
{
}

Timestamp Poller::poll(int timeoutMs, ChannelList* activeChannels)
{
	int numEvents = ::poll(pollfds_.data(), pollfds_.size(), timeoutMs);
	Timestamp now(Timestamp::now());

	if(numEvents > 0) {
		std::cout << "[Poller::poll]" << numEvents << " events happend\n";
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
			assert(channel->fd() == it->fd);
			channel->set_revents(it->revents);

			activeChannels->push_back(channel);
		}
	} // end for
}

void Poller::updateChannel(Channel* channel)
{
	assertInLoopThread();
	std::cout << "fd:[" << channel->fd()
		<< "] events:[" << channel->events() << "]\n";

	if(channel->index() == -1)
	{ // add new
		assert( channels_.find(channel->fd()) == channels_.end() );

		struct pollfd pfd;
		pfd.fd = channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		pollfds_.push_back(pfd);

		int idx = static_cast<int>(pollfds_.size()) - 1;
		channel->set_index(idx);
		channels_[pfd.fd] = channel;
	}
	else
	{ // update exists one
		assert(channels_.find(channel->fd()) != channels_.end());
		assert(channels_[channel->fd()] == channel);

		int idx = channel->index();
		assert(0 <= idx && idx < static_cast<int>(pollfds_.size()) );

		struct pollfd& pfd = pollfds_[idx];
		assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd()-1);
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		if(channel->isNoneEvent()) {
			// ignore this fd
			pfd.fd = -channel->fd()-1; // or ~fd - 1
		}
	}
}

void Poller::removeChannel(Channel* channel)
{
	assertInLoopThread();
	std::cout << "fd = " << channel->fd();
	assert(channels_.find(channel->fd()) != channels_.end());
	assert(channels_[channel->fd()] == channel);
	assert(channel->isNoneEvent());

	int idx = channel->index();
	assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
	const struct pollfd& pfd = pollfds_[idx];
	assert(pfd.fd == -channel->fd()-1 && pfd.events == channel->events()); (void)pfd;
	size_t n = channels_.erase(channel->fd());
	assert(n == 1); (void)n;

	if (static_cast<size_t>(idx) == pollfds_.size()-1) {
		pollfds_.pop_back();
	}
	else {
		int channelAtEnd = pollfds_.back().fd;
		std::iter_swap(pollfds_.begin() + idx, pollfds_.end() -1);
		if (channelAtEnd < 0)
			channelAtEnd = -channelAtEnd-1;
		channels_[channelAtEnd]->set_index(idx);
		pollfds_.pop_back();
	}
}
























