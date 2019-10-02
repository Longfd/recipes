#ifndef POLLER_H
#define POLLER_H

#include <map>
#include <vector>

#include "../base/Noncopyable.h"
#include "EventLoop.h"

class Poller : Noncopyable
{
public:
	Poller(EventLoop* loop);
	virtual ~Poller();

private:
	EventLoop* ownerLoop_;
};

#endif // POLLER_H















