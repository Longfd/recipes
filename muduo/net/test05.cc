#include "InetAddress.h"
#include "TcpServer.h"
#include "EventLoop.h"
#include "Buffer.h"
#include "../base/TimeStamp.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

void onConnection(const TcpConnectionPtr& conn)
{
	if (conn->connected())
	{
		printf("onConnection(): new connection [%s] from:%s\n",
			   conn->name().c_str(), 
			   conn->peerAddress().toHostPort().c_str());
	}
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime)
{
	printf("onMessage(): received %zd bytes from connection [%s]:\n"
		   "%s", buf->readableBytes(), conn->name().c_str(), receiveTime.toString().c_str());
}


int main()
{
	printf("main(): pid = %d\n", getpid());

	InetAddress servaddr(6000);
	EventLoop loop;
	TcpServer server(&loop, servaddr);
	server.setConnectionCallback(onConnection);
	server.setMessageCallback(onMessage);
	server.start();

	loop.loop();
}










