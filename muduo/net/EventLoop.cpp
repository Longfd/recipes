#include "EventLoop.h"
#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <poll.h>

// belongs to currentthread who first create the Eventloop obj;
__thread EventLoop* t_loopInThisThread = 0;

EventLoop::EventLoop() 
	:looping_(false),
	thredId_(CurrentThread::gettid())
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
	assertInLoopThread();
	looping_ = true;

	::poll(NULL, 0, 5 * 1000);

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












