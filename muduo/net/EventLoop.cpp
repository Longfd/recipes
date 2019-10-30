#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"
#include "TimerQueue.h"

#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <poll.h>
#include <functional>
#include <sys/eventfd.h>

// belongs to currentthread who first create the Eventloop obj;
// "__thread" key words avoid one thread have more than one EventLoop obj;
// gettid() avoid EventLoop obj called by another thread
__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;

static int createEventfd()
{
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0)
	{
		std::cout << "eventfd() Failed!\n";
		abort();
	}
	return evtfd;
}

EventLoop::EventLoop() 
	:looping_(false),
	quit_(false),
	callingPendingFunctors_(false),
	thredId_(CurrentThread::gettid()),
	pollReturnTime_(Timestamp()),
	poller_(new Poller(this)),
	timerQueue_(new TimerQueue(this)),
	wakeupfd_(createEventfd()),
	wakeupChannel_(new Channel(this, wakeupfd_))
{
	std::cout << "[INFO]EventLoop created at: " << this << " thredId_: " << thredId_ << '\n';
	if(t_loopInThisThread)
	{
		std::cout << "[FATAL]Another EventLoop " << t_loopInThisThread
			<< " exists in this thread " << thredId_ << '\n';
		exit(1);
	}
	else
	{
		t_loopInThisThread = this;
	}

	wakeupChannel_->setReadCallback(
			std::bind(&EventLoop::handleRead, this));
	wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() 
{
	assert(!looping_);	
	t_loopInThisThread = NULL;
	::close(wakeupfd_);
}

void EventLoop::loop()
{
	assert(!looping_);
	assertInLoopThread(); // loop must execute at I/O thread where EventLoop obj was created
	looping_ = true;
	quit_ = false;

	//::poll(NULL, 0, 5 * 1000);
	while (!quit_)
	{
		activeChannels_.clear();
		pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);

		for (auto it : activeChannels_) {
			it->handleEvent();
		}

		doPendingFunctors();
	}

	std::cout << "[LOG_TRACE]EventLoop " << this << " stop looping\n";
	looping_ = false;
}

void EventLoop::abortNotInLoopThread()
{
	std::cout << "[FATAL]Another EventLoop " << t_loopInThisThread
		<< " exists in this thread " << thredId_ << '\n';
	exit(1);
}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
	return t_loopInThisThread;
}

void EventLoop::quit()
{
	quit_ = true;
}

void EventLoop::updateChannel(Channel* theChannel)
{
	assert(theChannel->ownerLoop() == this);
	assertInLoopThread();
	poller_->updateChannel(theChannel);
}

void EventLoop::runInLoop(const Functor& cb)
{
	if (isInLoopThread())
	{
		cb();
	}
	else 
	{
		queueInLoop(cb);
	}
}

void EventLoop::queueInLoop(const Functor& cb)
{
	{
		MutexLockGuard lockGuard(mutex_);
		pendingFunctors_.push_back(cb);
	}

	if ( !isInLoopThread() || callingPendingFunctors_)
	{
		wakeup();
	}
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb)
{
	return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::now(), delay));
	return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::now(), interval));
	return timerQueue_->addTimer(cb, time, interval);
}

void EventLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = ::write(wakeupfd_, &one, sizeof(one));
	if (n != sizeof(one) )
	{
		std::cout << "EventLoop::wakeup() write Error!\n";
	}
}

void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = ::read(wakeupfd_, &one, sizeof(one) );
	if (n != sizeof(one))
	{
		std::cout << "EventLoop::handleRead() read Error!\n";
	}
}

void EventLoop::doPendingFunctors()
{
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;

	{
		MutexLockGuard lockGuard(mutex_);
		functors.swap(pendingFunctors_);
	}

	for (const Functor& call : functors)
	{
		call();
	}
	callingPendingFunctors_ = false;
}

void EventLoop::removeChannel(Channel* channel)
{
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller_->removeChannel(channel);
}







