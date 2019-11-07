#include "EventLoop.h"
#include "InetAddress.h"
#include "TcpClient.h"

#include <stdio.h>
#include <unistd.h>
#include <utility>
#include <functional>


std::string message = "Hello";

void onConnection(const TcpConnectionPtr& conn)
{
	if (conn->connected())
	{
		printf("test09.cc onConnection(): new connection [%s] from %s\n",
				conn->name().c_str(), conn->peerAddress().toHostPort().c_str()); 
		conn->send(message);
	}
	else
	{
		printf("test09.cc onConnection(): connection [%s] is down\n",
				conn->name().c_str());
	}
}

void onMessage(const TcpConnectionPtr& conn,
		Buffer* buf,
		Timestamp receiveTime)
{
	printf("test09.cc onMessage(): received %zd bytes from connection [%s] at %s\n",
			buf->readableBytes(),
			conn->name().c_str(),
			receiveTime.toString().c_str());

	printf("test09.cc onMessage(): [%s]\n", buf->retrieveAsString().c_str());
}

int main()
{
	EventLoop loop;
	InetAddress serverAddr("127.0.0.1", 6000);
	TcpClient client (&loop, serverAddr);

	client.setConnectionCallback(onConnection);
	client.setMessageCallback(onMessage);
	client.enableRetry();
	client.connect();

	loop.loop();
}












