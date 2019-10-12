#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"

#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <poll.h>

// belongs to currentthread who first create the Eventloop obj;
// "__thread" key words avoid one thread have more than one EventLoop obj;
// gettid() avoid EventLoop obj called by another thread
__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;

EventLoop::EventLoop() 
	:looping_(false),
	thredId_(CurrentThread::gettid()),
	poller_(new Poller(this))
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
}

EventLoop::~EventLoop() 
{
	assert(!looping_);	
	t_loopInThisThread = NULL;
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
		poller_->poll(kPollTimeMs, &activeChannels_);

		for (auto it : activeChannels_) {
			it->handleEvent();
		}
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








