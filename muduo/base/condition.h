#ifndef CONDITION_H
#define CONDITION_H

//#define NDEBUG // assert switch

#include <pthread.h>
#include <assert.h>
#include "noncopyable.h"
#include "mutex.h"

class Condition : noncopyable
{
public:
	explicit Condition(MutexLock& theMutex) {
		assert(0 == pthread_cond_init(&cond_, NULL));
	}
	~Condition() {
		assert(0 == pthread_cond_destroy(&cond_));
	}

	void wait() {
		assert(0 == pthread_cond_wait(&cond_, mutex_.getMutex()));
	}
	void notify() {
		assert(0 == pthread_cond_signal(&cond_));
	}
	void notifyAll() {
		assert(0 == pthread_cond_broadcast(&cond_));
	}

private:
	MutexLock& mutex_;
	pthread_cond_t cond_;
};

#endif

