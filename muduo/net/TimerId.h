#ifndef TIMERID_H
#define TIMERID_H

// This is a public header file, it must only include public header files.

class Timer;

class TimerId
{
	/// An opaque identifier, for canceling Timer.

	public:

		TimerId()
			: timer_(NULL),
			sequence_(0)
	{
	}

		TimerId(Timer* timer, int64_t seq)
			: timer_(timer),
			sequence_(seq)
	{
	}

		// default copy-ctor, dtor and assignment are okay

		friend class TimerQueue;

	private:
		Timer* timer_;
		int64_t sequence_;
};

#endif // TIMERID_H























