#include "Channel.h"
#include "EventLoop.h"
#include <iostream>
#include <sys/timerfd.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>


EventLoop* g_eventLoop = 0;
int timerfd;

void timeout()
{
	static int count = 0;
	uint64_t exp;

	read(timerfd, &exp, sizeof(exp));
	std::cout << "exp: " << exp << ", count: " << count << std::endl;
	count += exp;

	if(count == 5) {
		g_eventLoop->quit();
		std::cout << "Timeout! EventLoop Quit!\n";
	}
}

int main(int argc, char* argv[])
{
	EventLoop loop;
	g_eventLoop = &loop;

	timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	Channel channel(g_eventLoop, timerfd);
	channel.setReadCallback(timeout);
	channel.enableReading();

	struct itimerspec howlong;
	bzero(&howlong, sizeof(howlong));	
	howlong.it_value.tv_sec = 1;
	howlong.it_value.tv_nsec = 0;
	howlong.it_interval.tv_sec = 1;
	howlong.it_interval.tv_nsec = 0;
	timerfd_settime(timerfd, 0, &howlong, NULL);

	g_eventLoop->loop();

	close(timerfd);

	return 0;
}



























