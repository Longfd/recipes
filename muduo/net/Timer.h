#ifndef TIMER_H
#define TIMER_H

// This is an internal header file, you should not include this.

#include "../base/Noncopyable.h"
#include "../base/Atomic.h"
#include "../base/TimeStamp.h"
#include "Callbacks.h"

class Timer : Noncopyable
{
public:
	Timer(TimerCallback cb, TimeStamp when, double interval)
		: callback_(std::move(cb)),
		  expiration_(when),
		  interval_(interval),
		  repeat_(interval_ > 0.0),
		  sequence_(s_numCreated_.incrementAndGet())
	{}

	void run() const 
	{
		callback_;
	}

	TimeStamp expiration() const { return expiration_; }
	bool repeat() const { return repeat_; }
	int64_t sequence() const { return sequence_; }

	void restart(TimeStamp now);

	static int64_t numCreated() const { return s_numCreated_.get(); }

private:
	const TimerCallback callback_;
	TimeStamp expiration_;
	const double interval_;
	const bool repeat_;
	const int64_t sequence_;

	static AtomicInt64 s_numCreated_;
};

#endif // TIMER_H


































