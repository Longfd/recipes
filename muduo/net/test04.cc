#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "SocketOps.h"
#include <stdio.h>


void newConnCallback(int sockfd, const InetAddress& peerAddr)
{
	printf("new connection arrived: %s\n", peerAddr.toHostPort().c_str());
	::write(sockfd, "How are you?\n", 13);
	sockets::close(sockfd);
}

int main()
{
	printf("main(): pid = %d\n", getpid());

	InetAddress listenAddr(6000);
	EventLoop loop;

	Acceptor acceptor(&loop, listenAddr);
	acceptor.setNewConnectionCallback(newConnCallback);
	acceptor.listen();
	
	loop.loop();
}

