#include "TcpConnection.h"

#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"

#include <utility>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <iostream>


TcpConnection::TcpConnection(EventLoop* loop,
						const std::string& name,
						int sockfd,
						const InetAddress& localAddr,
						const InetAddress& peerAddr)
	: loop_(loop),
	  name_(name),
	  state_(kConnecting),
	  socket_(new Socket(sockfd)),
	  channel_(new Channel(loop, sockfd)),
	  localAddr_(localAddr),
	  peerAddr_(peerAddr)
{
	std::cout << "TcpConnection::ctor[" << name << "] at" << this 
			  << " fd = " << sockfd << std::endl;
	channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
}

TcpConnection::~TcpConnection()
{
	std::cout << "TcpConnection::dtor[" << name_ << "] at" << this 
			  << " fd = " << channel_->fd() << std::endl;
}

void TcpConnection::connectEstablished()
{
	loop_->assertInLoopThread();
	assert(state_ == kConnecting);	
	setState(kConnected);
	channel_->enableReading();

	connectionCallback_(shared_from_this());
}

void TcpConnection::handleRead()
{
	char buf[65536];
	ssize_t n = ::read(channel_->fd(), buf, sizeof(buf));
	messageCallback_(shared_from_this(), buf, n);
}



















