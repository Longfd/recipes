#ifndef EPOLLER_H
#define EPOLLER_H

#include "../base/TimeStamp.h"
#include "../base/Noncopyable.h"
#include "EventLoop.h"

#include <map>
#include <vector>

struct epoll_event;

class Channel;

class EPoller : Noncopyable
{
public:
	typedef std::vector<Channel*> ChannelList;

	EPoller(EventLoop* loop);
	~EPoller();

	Timestamp poll(int timeoutMs, ChannelList* activeChannels);

	void updateChannel(Channel* channel);

	void removeChannel(Channel* channel);

	void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }

private:
	typedef std::vector<struct epoll_event> EventList;
	typedef std::map<int, Channel*> ChannelMap;

	static const int kInitEventListSize = 16;

	void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

	void update(int operation, Channel* channel);

	EventLoop* ownerLoop_;
	int epollfd_;
	EventList events_;
	ChannelMap channels_;
};

#endif // EPOLLER_H





















