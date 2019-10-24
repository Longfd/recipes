#include "../base/CurrentThread.h"
#include "EventLoop.h"
#include "EventLoopThread.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


void callbackFunc()
{
	printf("runInThread(): pid = %d, tid = %d\n",
			getpid(), CurrentThread::gettid());
}

int main()
{
	printf("runInThread(): pid = %d, tid = %d\n",
			getpid(), CurrentThread::gettid());

	EventLoopThread loopThread;
	EventLoop* loop = loopThread.startLoop();
	loop->runInLoop(callbackFunc);
	sleep(1);
	loop->runAfter(2, callbackFunc);
	sleep(3);
	loop->quit();

	printf("exit main().\n");
}




















