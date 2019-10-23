#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <inttypes.h>
#include <string>


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

	std::string toString() const 
	{
		char buf[32] = {0};
		int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
		int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
		snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
		return buf;
	}

	int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }


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

