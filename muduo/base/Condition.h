#ifndef CONDITION_H
#define CONDITION_H

#include "Mutex.h"

class Condition : noncopyable 
{
public:
	Condition(Mutex& theMutex) 
		:mutex_(theMutex)
	{
		assert(pthread_cond_init(&cond_, NULL) == 0);
	}
	~Condition() {
		assert(pthread_cond_destroy(&cond_) == 0);
	}

	void wait() { assert(pthread_cond_wait(&cond_, mutex_.getPthreadMutex()) == 0); }
	void notify() { assert(pthread_cond_signal(&cond_) == 0); }
	void notifyAll() { assert(pthread_cond_broadcast(&cond_) == 0); }

private:
	Mutex& mutex_;
	pthread_cond_t cond_;
};

#endif // CONDITION_H

