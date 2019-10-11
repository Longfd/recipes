#ifndef POLLER_H
#define POLLER_H
// I/O mutilplexing with poll(2)
// this class dosn't own the channel objects

#include <map>
#include <vector>

#include "../base/Noncopyable.h"
#include "../base/TimeStamp.h"
#include "EventLoop.h"

struct pollfd;

class Channel;
class Poller : Noncopyable
{
public:
	typedef std::vector<Channel*> ChannelList;

	Poller(EventLoop* loop);
	~Poller();

	// Poll the I/O events
	// Must be called in the loop thread
	TimeStamp poll(int timeoutMs, ChannelList* activeChannels);
	
	// changes the interested I/O events
	// Must be called in the loop thread
	
	// add new channel with fd, or update events of fd in channel
	void updateChannel(Channel* channel);
	void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

	void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }

private:
	typedef std::vector<struct pollfd> PollFdList;
	typedef std::map<int, Channel*> ChannelMap;

	EventLoop* ownerLoop_;
	PollFdList pollfds_;
	ChannelMap channels_;
};

#endif // POLLER_H















