// solve the problem of the file "dead_lock.cpp"

#include <stdio.h>
#include <set>
#include <unistd.h>
#include <memory>
#include "mutex.h"

class Request;
class Inventory
{
public:
	Inventory() {
		pRequests_ = SetPtr(new ReqSet);
	}
	void add(Request* req) {
		MutexLockGuard lock(mutex_);
		if(!pRequests_.unique()) {
			pRequests_.reset(new ReqSet(*pRequests_));
		}
		(*pRequests_).insert(req);
	}
	void remove(Request* req) {
		MutexLockGuard lock(mutex_);
		if(!pRequests_.unique())
			pRequests_.reset(new ReqSet(*pRequests_));
		pRequests_->erase(req);
	}
	void printAll() const;

private:
	mutable MutexLock mutex_;

	// use shared_ptr
	typedef std::set<Request*> ReqSet;
	typedef std::shared_ptr<ReqSet> SetPtr;
	SetPtr pRequests_;
};

Inventory g_inventory;

class Request
{
public:
	~Request() __attribute__ ((noinline))
	{
		MutexLockGuard lock(mutex_);
		sleep(1);
		g_inventory.remove(this);
		printf("Request::~Request end\n");
	}
	void process() {
		MutexLockGuard lock(mutex_);
		g_inventory.add(this);
	}
	void print() const __attribute__ ((noinline)) {
		MutexLockGuard lock(mutex_);
		printf("Request::print() end\n");
	}

private:
	mutable MutexLock mutex_;
};

void Inventory::printAll() const {
	std::shared_ptr<ReqSet> shared_obj;
	{
		MutexLockGuard lock(mutex_);
		sleep(1);
		shared_obj = pRequests_;
	}

	std::set<Request*>::const_iterator it = shared_obj->begin();
	for (; it != shared_obj->end(); ++it) {
		(*it)->print();
	}
	printf("Inventory::printAll unlocked!\n");
}

void* thread_routine(void*)
{
	Request* req = new Request;
	req->process();
	sleep(3);
	delete req;
	pthread_detach(pthread_self());
}

int main(int argc, char* argv[])
{
	pthread_t pid;
	assert( 0 == pthread_create(&pid, NULL, thread_routine, NULL));
	usleep(500 * 1000);
	g_inventory.printAll();
}

