#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "../base/Noncopyable.h"
#include "../base/CurrentThread.h"

#include <memory>
#include <vector>

class Channel;
class Poller;
class EventLoop : Noncopyable
{
public:
	EventLoop();
	~EventLoop();

	void loop();
	void quit();

	// avoid another thread use the eventloop of currentthread
	bool isInLoopThread() const {
		return thredId_ == CurrentThread::gettid();
	}

	void assertInLoopThread() {
		if(!isInLoopThread()) {
			abortNotInLoopThread();
		}
	}

	EventLoop* getEventLoopOfCurrentThread();

	// internal usage
	void updateChannel(Channel* theChannel);

private:
	typedef std::vector<Channel*> ChannelList;
	
	void abortNotInLoopThread();

	bool looping_;
	bool quit_;
	const pid_t thredId_; // initialized at constructor
	std::unique_ptr<Poller> poller_;
	ChannelList activeChannels_;
};

#endif // EVENTLOOP_H

