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

	static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
	int64_t microSecondsSinceEpoch_;
};










#endif // TIMESTAMP_H

