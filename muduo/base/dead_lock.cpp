// test dead lock

#include <stdio.h>
#include <set>
#include <unistd.h>
#include "mutex.h"

class Request;
class Inventory
{
public:
	void add(Request* req) {
		MutexLockGuard lock(mutex_);
		requests_.insert(req);
	}
	void remove(Request* req) {
		MutexLockGuard lock(mutex_);
		requests_.erase(req);
	}
	void printAll() const;

private:
	mutable MutexLock mutex_;
	std::set<Request*> requests_;
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
	}
	void process() {
		MutexLockGuard lock(mutex_);
		g_inventory.add(this);
	}
	void print() const __attribute__ ((noinline)) {
		MutexLockGuard lock(mutex_);
	}

private:
	mutable MutexLock mutex_;
};

void Inventory::printAll() const {
	MutexLockGuard lock(mutex_);
	sleep(1);
	std::set<Request*>::const_iterator it = requests_.begin();
	for (; it != requests_.end(); ++it) {
		(*it)->print();
	}
	printf("Inventory::printAll unlocked!\n");
}

void* thread_routine(void*)
{
	Request* req = new Request;
	req->process();
	delete req;
	pthread_detach(pthread_self());
}

int main(int argc, char* argv[])
{
	pthread_t pid;
	assert( 0 == 
			pthread_create(&pid, NULL, thread_routine, NULL));
	usleep(500 * 1000);
	g_inventory.printAll();
}

