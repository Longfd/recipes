#ifndef MUTEX_H
#define MUTEX_H

//#define NDEBUG // assert switch

#include <pthread.h>
#include <assert.h>
#include "noncopyable.h"

class MutexLock : noncopyable
{
public:
	MutexLock() {
		assert(0 == pthread_mutex_init(&mutex_, NULL));
	}
	~MutexLock() {
		assert(0 == pthread_mutex_destroy(&mutex_));
	}

	void lock() {
		assert(0 == pthread_mutex_lock(&mutex_));
	}
	void unlock() {
		assert(0 == pthread_mutex_unlock(&mutex_));
	}

private:
	pthread_mutex_t mutex_;
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

#endif
