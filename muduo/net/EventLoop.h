#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "../base/Noncopyable.h"
#include "../base/CurrentThread.h"
#include "../base/TimeStamp.h"
#include "Callbacks.h"
#include "TimerId.h"

#include <memory>
#include <vector>

class Channel;
class Poller;
class TimerQueue;
class EventLoop : Noncopyable
{
public:
	EventLoop();
	~EventLoop();

	void loop();
	void quit();

	// Time When poll returns, usually means data arrival
	Timestamp pollReturnTime() const { return pollReturnTime_; };

	void runInLoop(const Functor& cb);

	void queueInLoop(const Functor& cb);

	// Timers
	TimerId runAt(const Timestamp& time, const TimerCallback& cb);
	TimerId runAfter(double delay, const TimerCallback& cb);
	TimerId runEvery(double interval, const TimerCallback& cb);

	// void cancel(TimerId timerId);
	
	// internal use only
	void wakeup();
	void updateChannel(Channel* channel);
	// void removeChannel(Channel* channel);

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
	void handleRead(); // wake up
	void doPendingFunctors();

	bool looping_;
	bool quit_;
	bool callingPendingFunctors_; /* atomic */
	const pid_t thredId_; // initialized at constructor
	Timestamp pollReturnTime_;
	std::unique_ptr<Poller> poller_;
	std::unique_ptr<TimerQueue> timerQueue_;
	int wakeupfd_;
	std::unique_ptr<Channel> wakeupChannel_;
	ChannelList activeChannels_;
	Mutex mutex_;
	std::vector<Functor> pendingFunctors_;// Guarded By mutex_
};

#endif // EVENTLOOP_H

