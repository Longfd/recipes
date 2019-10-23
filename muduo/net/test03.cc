#include "EventLoop.h"
#include "EventLoopThread.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


void callbackFunc()
{
	printf("runInThread(): pid = %d, tid = %d\n",
			getpid(), muduo::CurrentThread::tid());
}

int main()
{
	printf("runInThread(): pid = %d, tid = %d\n",
			getpid(), muduo::CurrentThread::tid());

	muduo::EventLoopThread loopThread;
	EventLoop* loop = loopThread.startLoop();
	loop->runInLoop(runInThread);
	sleep(1);
	loop->runAfter(2, runInThread);
	sleep(3);
	loop->quit();

	printf("exit main().\n");
}




















