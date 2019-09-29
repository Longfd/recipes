#include <iostream>
#include <unistd.h>
#include "Mutex.h"
#include "Condition.h"

Mutex mutex_;
Condition cond(mutex_);
int count = 1;

void* thread_routine(void* arg)
{
	MutexLockGuard guard(mutex_);
	sleep(2);
	if((--count) == 0)
		cond.notifyAll();

	pthread_detach(pthread_self());
	return NULL;
}


int main()
{
	pthread_t tid;

	assert(pthread_create(&tid, NULL, thread_routine, NULL) == 0);
	//assert(pthread_join(tid, NULL) == 0);
	//printf("thread[%u] exit success!\n", tid);
	
	// wait for condition
	while (1) 
	{
		MutexLockGuard guard(mutex_);

		if(count == 0) {
			printf("count == 0 \n");
			return 0;
		}
		else {
			std::cout << "cond.wait()...\n";
			cond.wait();
		}
	}

	return 0;
}

