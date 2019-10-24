#include "TimerQueue.h"

#include "Timer.h"
#include "TimerId.h"
#include "EventLoop.h"
#include "Callbacks.h"
#include "../base/IgnoreUnusedWarning.h"

#include <sys/timerfd.h>
#include <unistd.h>
#include <iostream>
#include <strings.h>
#include <functional>
#include <utility>
#include <assert.h>
#include <algorithm>


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
	//timerfdChannel_.remove();
	::close(timerfd_);
	// do not remove channel, since we're in EventLoop::dtor();
	for (const Entry& timer : timers_)
	{
		delete timer.second;
	}
}

TimerId TimerQueue::addTimer(TimerCallback cb, 
				 Timestamp when, 
				 double interval)
{
	Timer* timer = new Timer(std::move(cb), when, interval);
	loop_->runInLoop(
			std::bind(&TimerQueue::addTimerInLoop, this, timer));
	return TimerId(timer, timer->sequence());
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
	loop_->assertInLoopThread();
	bool earliestChanged = insert(timer);

	if (earliestChanged)
	{
		resetTimerfd(timerfd_, timer->expiration());
	}
}

void TimerQueue::cancel(TimerId timerId)
{
	loop_->runInLoop(
			std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
	loop_->assertInLoopThread();
	assert(timers_.size() == activeTimers_.size());
	ActiveTimer timer(timerId.timer_, timerId.sequence_);
	ActiveTimerSet::iterator it = activeTimers_.find(timer);
	if (it != activeTimers_.end())
	{
		size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
		assert(n == 1);
		IntendToIgnoreThisVariable(n);
		delete it->first;
		activeTimers_.erase(it);
	}
	else if (callingExpiredTimers_)
	{
		cancelingTimers_.insert(timer);
	}

	assert(timers_.size() == activeTimers_.size());
}

void TimerQueue::handleRead()
{
	loop_->assertInLoopThread();
	Timestamp now(Timestamp::now());
	readTimerfd(timerfd_, now);

	std::vector<Entry> expired = getExpired(now);

	callingExpiredTimers_ = true;
	cancelingTimers_.clear();
	// safe to callback outside critical section
	for (const Entry& it : expired)
	{
		it.second->run();
	}
	callingExpiredTimers_ = false;

	reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
	assert(timers_.size() == activeTimers_.size());
	std::vector<Entry> expired;
	Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
	TimerList::iterator end = timers_.lower_bound(sentry);
	assert(end == timers_.end() || now < end->first);
	std::copy(timers_.begin(), end, back_inserter(expired));
	timers_.erase(timers_.begin(), end);

	for (const Entry& it : expired)
	{
		ActiveTimer timer(it.second, it.second->sequence());
		int n = activeTimers_.erase(timer);
		assert(n == 1);
		IntendToIgnoreThisVariable(n);
	}

	assert(timers_.size() == activeTimers_.size());
	return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
	Timestamp nextExpire;

	for (const Entry& it : expired)
	{
		ActiveTimer timer(it.second, it.second->sequence());
		if (it.second->repeat() &&
			cancelingTimers_.find(timer) == cancelingTimers_.end())
		{
			it.second->restart(now);
			insert(it.second);
		}
		else
		{
			delete it.second;
		}
	}

	if (!timers_.empty())
	{
		nextExpire = timers_.begin()->second->expiration();
	}

	if (nextExpire.valid())
	{
		resetTimerfd(timerfd_, nextExpire);
	}
}

bool TimerQueue::insert(Timer* timer)
{
	loop_->assertInLoopThread();
	assert(timers_.size() == activeTimers_.size());

	bool earliestChanged = false;
	Timestamp when = timer->expiration();
	TimerList::iterator it = timers_.begin();
	if(it == timers_.end() || when < it->first) {
		earliestChanged = true;
	}

	{
		std::pair<TimerList::iterator, bool> result = 
			timers_.insert(Entry(when, timer));
		assert(result.second == true);
		// assert() function only valid when DEBUG, otherwise, result will be 
		// not unused, so the gcc compiler will warning
		// use the method below to avoid warning
		IntendToIgnoreThisVariable(result); 
	}

	{
		std::pair<ActiveTimerSet::iterator, bool> result 
			= activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
		assert(result.second);
		IntendToIgnoreThisVariable(result);
	}

	assert(timers_.size() == activeTimers_.size());
	return earliestChanged;
}

