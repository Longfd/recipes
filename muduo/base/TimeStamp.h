#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <stdint.h>


class Timestamp 
{
public:

	Timestamp()
		:microSecondsSinceEpoch_(0)
	{
	}
	explicit Timestamp(int64_t microSecondsSinceEpochArg)
		: microSecondsSinceEpoch_(microSecondsSinceEpochArg)
	{
	}

	int64_t getTime() const { return microSecondsSinceEpoch_; }

	// get time of now.
	static Timestamp now();

	static Timestamp invalid() 
	{ 
		return Timestamp();
 	}


	static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
	int64_t microSecondsSinceEpoch_;
};


inline Timestamp addTime(Timestamp timestamp, double seconds)
{
	int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
	return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}








#endif // TIMESTAMP_H

