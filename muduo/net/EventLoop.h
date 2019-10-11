#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "../base/Noncopyable.h"
#include "../base/CurrentThread.h"

class Channel;
class EventLoop : Noncopyable
{
public:
	EventLoop();
	~EventLoop();

	void loop();

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
	void abortNotInLoopThread();

	bool looping_;
	const pid_t thredId_; // initialized at constructor
};

#endif // EVENTLOOP_H

