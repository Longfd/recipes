#include "TimerQueue.h"

#include "Timer.h"
#include "TimerId.h"
#include "EventLoop.h"

#include <sys/timerfd.h>
#include <unistd.h>
#include <iostream>
#include <strings.h>
#include <functional>



int createTimerfd()
{
	int timerfd = ::timerfd_create(CLOCK_MONOTONIC, 
			TFD_NONBLOCK | TFD_CLOEXEC);
	if(timerfd < 0)
	{
		std::cout << "Failed in timerfd_create";
	}

	return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp when)
{
	int64_t microseconds = when.microSecondsSinceEpoch() 
		- Timestamp::now().microSecondsSinceEpoch();

	if (microseconds < 100)
		microseconds = 100;

	struct timespec ts;
	ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
	ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
	return ts;
}

void readTimerfd(int timerfd, Timestamp now)
{
	uint64_t howmany;
	ssize_t n = :: read(timerfd, &howmany, sizeof(howmany));
	if (n != sizeof(howmany)) {
		std::cout << "TimerQueue::handleRead() reads " << n << " bytes instead of 8\n";
	}
	std::cout << "TimerQueue::handleRead()" << howmany << "at" << now.toString();
}

void resetTimerfd(int timerfd, Timestamp expiration)
{
	// wake up loop by timerfd_settime
	struct itimerspec newValue;
	struct itimerspec oldValue;
	bzero(&newValue, sizeof(newValue));
	bzero(&oldValue, sizeof(oldValue));

	newValue.it_value = howMuchTimeFromNow(expiration);
	int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
	if (ret)
	{
		std::cout << "timerfd_settime() Failed!\n";
	}
}

	TimerQueue::TimerQueue(EventLoop* loop)
: loop_(loop),
	timerfd_(createTimerfd()),
	timerfdChannel_(loop, timerfd_),
	timers_(),
	callingExpiredTimers_(false)
{
	timerfdChannel_.setReadCallback(
			std::bind(&TimerQueue::handleRead, this));
	timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
	timerfdChannel_.disableAll();
	timerfdChannel_.remove();
	::close(timerfd_);
	// do not remove channel, since we're in EventLoop::dtor();
	for (const Entry& timer : timers_)
	{
		delete timer.second;
	}
}





























