#ifndef MUTEX_H
#define MUTEX_H

#include <sys/types.h>
#include <pthread.h>
#include <assert.h>
#include "Thread.h"
#include "Noncopyable.h"

class Mutex : noncopyable
{
public:
	Mutex() 
		:_holder(0)
	{ 
		assert(pthread_mutex_init(&_mutex, NULL) == 0); 
	}
	~Mutex() { 
		assert(_holder == 0);
		assert(pthread_mutex_destroy(&_mutex) == 0); 
	}

	void lock() { 
		assert(pthread_mutex_lock(&_mutex) == 0); 
		_holder = gettid();
	}
	void unlock() { 
		_holder = 0;
		assert(pthread_mutex_unlock(&_mutex) == 0); 
	}

	bool isLockedBythisThread() { return _holder == gettid(); };
	pthread_mutex_t* getPthreadMutex() { return &_mutex; }

private:
	pthread_mutex_t _mutex;
	pid_t _holder;
};

class MutexLockGuard : noncopyable
{
public:
	MutexLockGuard(Mutex& theMutex) 
		:_mutex(theMutex)
	{
		_mutex.lock();
	}
	~MutexLockGuard() { _mutex.unlock(); }

private:
	Mutex& _mutex;
};

#define MutexLockGuard(x) static_assert(false, "Missing guard object name")

#endif // MUTEX_H

