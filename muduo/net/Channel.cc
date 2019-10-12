#include "Channel.h"
#include "EventLoop.h"

//#define _GNU_SOURCE enable by default
#include <poll.h>
#include <iostream>


const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
	:loop_(loop),
	fd_(fd),
	events_(0),
	revents_(0),
	index_(-1)
{
}

void Channel::update()
{
	loop_->updateChannel(this);
}

void Channel::handleEvent()
{
	if (revents_ & POLLNVAL) {
		std::cout << "Channel::handleEvent() POLLNVAL!\n";
	}

	if (revents_ & (POLLNVAL | POLLERR) ) {
		if(errorCallback_) errorCallback_();
	}
	else if ( revents_ & (POLLIN | POLLRDHUP | POLLPRI) ) {
		if(readCallback_) readCallback_();
	}
	else if ( revents_ & POLLOUT ) {
		if(writeCallback_) writeCallback_();
	}
}











