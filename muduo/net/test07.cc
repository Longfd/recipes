#include "EventLoop.h"
#include "TcpServer.h"
#include "InetAddress.h"
#include <stdio.h>

void onConnection(const TcpConnectionPtr& conn)
{
	if (conn->connected())
	{
		printf("test07.cc onConnection(): tid:%d new connection [%s] from %s\n",
			   CurrentThread::gettid(), conn->name().c_str(), 
			   conn->peerAddress().toHostPort().c_str());
	}
	else
	{
		printf("test07.cc onConnection(): tid:%d connection [%s] is down\n",
			   CurrentThread::gettid(), conn->name().c_str());
	}
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime)
{
	printf("test07.cc onMessage(): tid:%d receive %zd bytes from connection [%s] at %s\n",
		   CurrentThread::gettid(), buffer->readableBytes(), 
		   conn->name().c_str(), receiveTime.toString().c_str());
	std::string recv = buffer->retrieveAsString();
	printf("recv:%s\n", recv.c_str());
	conn->send(recv);
}


int main(int argc, char *argv[])
{
	printf("main(): pid = %d\n", getpid());

	EventLoop loop;
	InetAddress servAddr(6000);
	TcpServer server(&loop, servAddr);
	server.setConnectionCallback(onConnection);
	server.setMessageCallback(onMessage);
	if (argc > 1) {
		server.setThreadNum(atoi(argv[1]));
	}
	server.start();

	loop.loop();
}






