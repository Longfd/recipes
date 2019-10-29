#include "TcpServer.h"

#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketOps.h"

#include <utility>
#include <stdio.h> // snprintf
#include <iostream>


TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
	: loop_(loop),
	  name_(listenAddr.toHostPort()),
	  acceptor_(new Acceptor(loop, listenAddr)),
	  started_(false),
	  nextConnId_(1)
{
	acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
	if (!started_)
	{
		started_ = true;
	}

	if (!acceptor_->listening())
	{
		loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_->get()));
	}
}

TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
	loop_->assertInLoopThread();
	
	char buf[32];
	snprintf(buf, sizeof(buf), "#%d", nextConnId_++);
	std::string connName = name_ + buf;

	std::cout << "TcpServer::newConnection [" << name_
			  << "] - new connection [" << connName
			  << "] from " << peerAddr.toHostPort();
	InetAddress localAddr(sockets::getLocalAddr(sockfd));

	TcpConnectionPtr conn(
			new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));
	connections_[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->connectEstablished();
}














