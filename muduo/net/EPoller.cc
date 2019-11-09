#include "EPoller.h"

#include "Channel.h"
#include "../base/IgnoreUnusedWarning.h"

#include <iostream>
#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h> // bzero


//	typedef union epoll_data {
//		void 	*ptr;
//		int  	 fd;
//		uint32_t u32;
//		uint64_t u64;
//	} epoll_data_t;
//
//	struct epoll_event {
//		uint32_t	 events; /* Epoll events */
//		epoll_data_t data; /* User data variable */
//	};


using std::cout; using std::endl;

static_assert(EPOLLIN == POLLIN, "EPOLLIN == POLLIN");
static_assert(EPOLLPRI == POLLPRI, "EPOLLPRI == POLLPRI");
static_assert(EPOLLOUT == POLLOUT, "EPOLLOUT == POLLOUT");
static_assert(EPOLLRDHUP == POLLRDHUP, "EPOLLRDHUP == POLLRDHUP");
static_assert(EPOLLERR == POLLERR, "EPOLLERR == POLLERR");
static_assert(EPOLLHUP == POLLHUP, "EPOLLHUP == POLLHUP");

namespace {
	const int kNew = -1;
	const int kAdded = 1;
	const int kDeleted = 2;
}

EPoller::EPoller(EventLoop* loop)
	: ownerLoop_(loop),
	  epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
	  events_(kInitEventListSize)
{
	if (epollfd_ < 0)
	{
		cout << "EPoller::EPoller() epollfd_ < 0\n";
		exit(EXIT_FAILURE);
	}
}

EPoller::~EPoller()
{
	::close(epollfd_);
}

Timestamp EPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
	int numEvents = epoll_wait(epollfd_, events_.data(),
							   static_cast<int>(events_.size()),
							   timeoutMs);
	Timestamp now(Timestamp::now());
	if (numEvents > 0)
	{
		cout << "EPoller::poll(): " << numEvents << " events_ happened\n";
		fillActiveChannels(numEvents, activeChannels);
		if (static_cast<size_t>(numEvents) == events_.size())
		{
			events_.resize(events_.size() * 2);
		}
	}
	else if (numEvents == 0)
	{
		cout << "EPoller::poll(): nothing happened\n";
	}
	else
	{
		cout << "EPoller::poll(): epoll_wait() error:" << errno << endl;
	}

	return now;
}

void EPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
	assert(static_cast<size_t>(numEvents) <= events_.size());
	for (int i = 0; i < numEvents; ++i)
	{
		Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
#ifndef NDEBUG
		int fd = channel->fd();
		ChannelMap::const_iterator it = channels_.find(fd);
		assert(it != channels_.end());
		assert(it->second == channel);
#endif
		channel->set_revents(events_[i].events);
		activeChannels->push_back(channel);
	}
}

void EPoller::updateChannel(Channel* channel)
{
	assertInLoopThread();
	cout << "fd = " << channel->fd() << " events = " << channel->events() << endl;

	const int index = channel->index();
	if (index == kNew || index == kDeleted)
	{
		// a new one, add with EPOLL_CTL_ADD
		int fd = channel->fd();
		if (index == kNew)
		{
			assert(channels_.find(fd) == channels_.end());
			channels_[fd] = channel;
		}
		else // index == kDeleted
		{
			assert(channels_.find(fd) != channels_.end());
			assert(channels_[fd] == channel);
		}
		channel->set_index(kAdded);
		update(EPOLL_CTL_ADD, channel);
	}
	else
	{
		update(EPOLL_CTL_MOD, channel);
	}
}

void EPoller::removeChannel(Channel* channel)
{
	assertInLoopThread();
	int fd = channel->fd();
	cout << "EPoller::removeChannel(): fd = " << fd << endl;
	assert(channels_.find(fd) != channels_.end());
	assert(channels_[fd] == channel);
	assert(channel->isNoneEvent());

	int index = channel->index();
	assert(index == kAdded || index == kDeleted);
	size_t n = channels_.erase(fd);
	assert(n == 1);IntendToIgnoreThisVariable(n);
	if ( index == kAdded)
	{
		update(EPOLL_CTL_DEL, channel);
	}
	channel->set_index(kNew);
}

void EPoller::update(int operation, Channel* channel)
{
	struct epoll_event event;
	bzero(&event, sizeof(event));
	event.events = channel->events();
	event.data.ptr = channel;
	int fd = channel->fd();
	if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
	{
		if (operation == EPOLL_CTL_DEL)
		{
			cout << "EPoller::update() error: epoll_ctl op = " << operation << " fd = " << fd << endl;
		}
		else
		{
			cout << "EPoller::update() error: epoll_ctl op = " << operation << " fd = " << fd << endl;
			exit(EXIT_FAILURE);
		}
	}
}























