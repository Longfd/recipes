#include "Channel.h"
#include "EventLoop.h"

//#define _GNU_SOURCE enable by default
#include <poll.h>
#include <iostream>
#include <assert.h>


const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
	:loop_(loop),
	fd_(fd),
	events_(0),
	revents_(0),
	index_(-1),
	eventHandling_(false)
{
}

Channel::~Channel()
{
	assert(!eventHandling_);	
}

void Channel::update()
{
	loop_->updateChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
	if (revents_ & POLLNVAL) {
		std::cout << "Channel::handleEvent() POLLNVAL!\n";
		return;
	}

	eventHandling_ = true;

	if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
		std::cout << "Channel::handleEvent() POLLHUP(output only)\n";
		if (closeCallback_) closeCallback_();
	}

	if (revents_ & (POLLNVAL | POLLERR) ) {
		if(errorCallback_) errorCallback_();
	}
	else if ( revents_ & (POLLIN | POLLRDHUP | POLLPRI) ) {
		if(readCallback_) readCallback_(receiveTime);
	}
	else if ( revents_ & POLLOUT ) {
		if(writeCallback_) writeCallback_();
	}

	eventHandling_ = false;
}











