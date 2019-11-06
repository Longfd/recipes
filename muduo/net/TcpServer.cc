#include "TcpServer.h"

#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketOps.h"
#include "EventLoopThreadPool.h"

#include <utility>
#include <stdio.h> // snprintf
#include <iostream>


TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
	: loop_(loop),
	  name_(listenAddr.toHostPort()),
	  acceptor_(new Acceptor(loop, listenAddr)),
	  threadPool_(new EventLoopThreadPool(loop)),
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
		threadPool_->start();
	}

	if (!acceptor_->listening())
	{
		loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
	}
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
	loop_->assertInLoopThread();
	
	char buf[32];
	snprintf(buf, sizeof(buf), "#%d", nextConnId_++);
	std::string connName = name_ + buf;

	std::cout << "TcpServer::newConnection [" << name_
			  << "] - new connection [" << connName
			  << "] from " << peerAddr.toHostPort();
	InetAddress localAddr(sockets::getLocalAddr(sockfd));

	EventLoop* ioLoop = threadPool_->getNextLoop();

	TcpConnectionPtr conn(
			new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr));
	connections_[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));
	ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
	loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
	loop_->assertInLoopThread();
	std::cout << "TcpServer::removeConnection() [" << name_
			  << "] - connection " << conn->name() << std::endl;
	size_t n = connections_.erase(conn->name());
	assert(n == 1); (void)n;
	EventLoop* ioLoop = conn->getLoop();
	ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}	

void TcpServer::setThreadNum(int threadNum)
{
	assert(threadNum >= 0);
	threadPool_->setThreadNum(threadNum);
}










