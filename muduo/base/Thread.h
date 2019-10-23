#ifndef THREAD_H
#define THREAD_H

#include "Noncopyable.h"
#include "Atomic.h"
#include "CountDownLatch.h"

#include <pthread.h>
#include <functional>
#include <memory>


class Thread : Noncopyable
{
public:
	typedef std::function<void()> ThreadFunc;

	explicit Thread(const ThreadFunc&, const std::string& name = std::string());
	~Thread() {}

	void start();
	void join();

	bool started() { return started_; }
	pid_t tid() const { return tid_; }
  	const string& name() const { return name_; }

	static int numCreated() { return numCreated_.get(); }

private:
	void setDefaultName();

	bool started_;
	bool joined_;
	pthread_t pthreadId_;
	pid_t tid_;
	ThreadFunc func_;
	std::string name_;
	CountDownLatch latch_;

	static AtomicInt32 numCreated_;
};


#endif // THREAD_H













