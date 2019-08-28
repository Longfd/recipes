#ifndef MUTEX_H
#define MUTEX_H

//#define NDEBUG // assert switch

#include <pthread.h>
#include <assert.h>
#include "noncopyable.h"

class MutexLock : noncopyable
{
public:
	MutexLock() :
		holder_(0)
	{
		assert(0 == pthread_mutex_init(&mutex_, NULL));
	}
	~MutexLock() {
		assert(holder_ == 0);
		assert(0 == pthread_mutex_destroy(&mutex_));
	}

	void lock() {
		assert(0 == pthread_mutex_lock(&mutex_));
		holder_ = pthread_self();
	}
	void unlock() {
		holder_ = 0;
		assert(0 == pthread_mutex_unlock(&mutex_));
	}
	bool isLockedByThisThread() {
		return holder_ == pthread_self();
	}

private:
	pthread_mutex_t mutex_;
	pthread_t holder_;
};

class MutexLockGuard : noncopyable
{
public:
	MutexLockGuard(MutexLock& theMutex) :
		mutex_(theMutex)
	{
		mutex_.lock();
	}

	~MutexLockGuard() {
		mutex_.unlock();
	}

private:
	MutexLock& mutex_;
};

// avoid case like this: Mutexlockguard(mutex_); --> it cannot lock mutex_;
#define MutexLockGuard(x) static_assert(false, "missing MutexLockGuard variable name");

#endif
