#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

#include "EventLoop.h"


void* thread_func(void* arg)
{
	printf("thread_func(): pid = %d, tid = %d\n",
			getpid(), CurrentThread::gettid());

	EventLoop loop;
	loop.loop();

	return NULL;
}

int main(int argc, char* argv[])
{
	printf("main(): pid = %d, tid = %d\n",
			getpid(), CurrentThread::gettid());
	EventLoop loop;
	loop.loop();

	pthread_t tid;
	if ( pthread_create(&tid, NULL, thread_func, NULL) != 0) {
		perror("pthread_create() error");
		exit(1);
	}
	if( pthread_join(tid, NULL) != 0) {
		perror("pthread_join() error");
		exit(1);
	}

	return 0;
}


