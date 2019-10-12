#include "Channel.h"
#include "EventLoop.h"
#include <iostream>
#include <sys/timerfd.h>
#include <time.h>
#include <string.h>


EventLoop* g_eventLoop = 0;

void timeout()
{
	std::cout << "Timeout! EventLoop Quit!\n";
	g_eventLoop->quit();
}

int main(int argc, char* argv[])
{
	EventLoop loop;
	g_eventLoop = &loop;

	int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	Channel channel(g_eventLoop, timerfd);
	channel.setReadCallback(timeout);
	channel.enableReading();

	struct itimerspec howlong;
	bzero(&howlong, sizeof(howlong));	
	howlong.it_value.tv_sec = 5;
	timerfd_settime(timerfd, 0, &howlong, NULL);

	g_eventLoop->loop();

	close(timerfd);

	return 0;
}



























