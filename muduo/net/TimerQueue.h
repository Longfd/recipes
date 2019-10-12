#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

// no guarantee that the callback will be on time
// schedules the callback to be run at given timerqueue
// repeats if interval > 0.0

#include "../base/Noncopyable.h"
#include "../base/TimeStamp.h"

#include <utility>
#include <set>
#include <vector>

class EventLoop;
class Channel;
class TimerQueue : Noncopyable
{
public:
	TimerQueue(EventLoop* loop);
	~TimerQueue();

	// must be thread safe, usually be called from other threads
	TimerId addTimer(const TimerCallback& cb, 
					 Timestamp when, double interval);

	void cancel(TimerId timerId);

	typedef std::pair<Timestamp, Timer*> Entry;
	typedef std::set<Entry> TimerList;

	// called when timer expire
	void handleRead();

	// move out expire timers
	std::vector<Entry> getExpired(Timestamp now);
	void reset(const std::vector<Entry>& expired, Timestamp now);

	bool insert(Timer* timer);

	EventLoop* loop_;
	const int timer_fd;
	Channel timerfdChannel_;
	TimerList timers_;
};

#endif // TIMERQUEUE_H

















