#ifndef TIMESTAMP_H
#define TIMESTAMP_H


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

private:
	int64_t microSecondsSinceEpoch_;
};










#endif // TIMESTAMP_H

