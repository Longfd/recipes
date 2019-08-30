// solve the race condition: call Request::print when object Request destruct
// imeplemented with shared_ptr, used to store Request obj

#include <stdio.h>
#include <set>
#include <unistd.h>
#include <memory>
#include "mutex.h"

class Request;
typedef std::shared_ptr<Request> spReq;
typedef std::set<spReq> ReqSet;
typedef std::shared_ptr<ReqSet> SetPtr;

class Inventory
{
public:
	Inventory() {
		pRequests_ = SetPtr(new ReqSet);
	}
	void add(spReq req) {
		MutexLockGuard lock(mutex_);
		if(!pRequests_.unique()) {
			pRequests_.reset(new ReqSet(*pRequests_));
		}
		(*pRequests_).insert(req);
		printf("Inventory::add: set size:%d\n", pRequests_->size());
	}
	void remove(spReq req) {
		MutexLockGuard lock(mutex_);
		if(!pRequests_.unique())
			pRequests_.reset(new ReqSet(*pRequests_));
		pRequests_->erase(req);
		printf("Inventory::remove: set size:%d\n", pRequests_->size());
	}
	void printAll() const;

private:
	mutable MutexLock mutex_;

	SetPtr pRequests_;
};

Inventory g_inventory;

class Request : public std::enable_shared_from_this<Request>
{
public:
	~Request() { printf("Request::~Request() end\n"); }


	void process() {
		MutexLockGuard lock(mutex_);
		g_inventory.add(shared_from_this());
	}
	void cancle() {
		MutexLockGuard lock(mutex_);
		g_inventory.remove(shared_from_this());
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
		shared_obj = pRequests_;
	}

	// wait for request destruct
	sleep(1);

	ReqSet::const_iterator it = shared_obj->begin();
	printf("Inventory::printAll: set.size():%d\n", shared_obj->size());
	for (; it != shared_obj->end(); ++it) {
		(*it)->print();
	}
	printf("Inventory::printAll unlocked!\n");
}

void* thread_routine(void*)
{
	spReq sp = std::make_shared<Request>();
	sp->process();
	// wait for print
	//sleep(2);
	sp->cancle();
	pthread_detach(pthread_self());
}

int main(int argc, char* argv[])
{
	pthread_t pid;
	assert( 0 == pthread_create(&pid, NULL, thread_routine, NULL));
	sleep(1);
	g_inventory.printAll();
	sleep(2);
	return 0;
}

