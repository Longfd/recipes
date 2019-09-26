#ifndef SINGLETON_H
#define SINGLETON_H

#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include "noncopyable.h"

template <typename T>
class Singleton : noncopyable
{
public:
	static T& getInstance() {
		assert(0 == pthread_once(&ponce_, Singleton::init));
	}
	static void init() {
		value_ = new T();
		atexit(destroy);
	}
	static void destroy() {
		delete value_;
		value_ = NULL;
	}

private:
	Singleton() {}
	~Singleton() {}

	static pthread_once_t ponce_;
	static T* value_;
};

template <typename T>
pthread_once_t Singleton::ponce_ = PTHREAD_ONCE_INIT;
template <typename T>
T* Singleton::value_ = NULL;

#endif

