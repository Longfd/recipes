#include "EventLoop.h"
#include "InetAddress.h"
#include "Connector.h"
#include <stdio.h>
#include <memory>


EventLoop* g_loop;

void connectCallback(int sockfd)
{
	printf("test08.cc connectCallback(): connected. sockfd:%d\n", sockfd);
	g_loop->quit();
}

int main()
{
	EventLoop loop;
	g_loop = &loop;
	InetAddress servAddr("127.0.0.1", 6000);
	ConnectorPtr connector = std::make_shared<Connector>(&loop, servAddr);	
	connector->setNewConnectionCallback(connectCallback);
	connector->start();
	
	loop.loop();
}







