#include "TcpConnection.h"

#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "SocketOps.h"

#include <utility>
#include <stdio.h>
#include <errno.h>
#include <string.h>
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

void TcpConnection::handleRead(Timestamp receiveTime)
{
	int saveErrno = 0;
	ssize_t n = inputBuffer_.readFd(channel_->fd(), &saveErrno);
	if (n > 0) {
		messageCallback_(shared_from_this(), inputBuffer_, receiveTime);
	}
	else if (n == 0) {
		handleClose();
	}
	else {
		errno = saveErrno;
		std::cout << "TcpConnection::handleRead() Error\n";
		handleError();
	}

}


void TcpConnection::handleWrite()
{
}

void TcpConnection::handleClose()
{
	loop_->assertInLoopThread();
	std::cout << "TcpConnection::handleClose state = " << state_ << std::endl;
	assert(state_ = kConnected);
	// we don't close fd, leave it to dtor, so we can find leaks easily
	channel_->disableAll();	
	// must be the last line
	closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
	int err = sockets::getSocketError(channel_->fd());
	std::cout << "TcpConnection::handleError() [" << name_ 
			  << "] - SO_ERROR = " << err << strerror(err) << "\n"; // FIXME strerror_r use thread variable to save buf
}

void TcpConnection::connectDestroyed()
{
	loop_->assertInLoopThread();
	assert(state_ == kConnected);
	setState(kDisconnected);
	channel_->disableAll();
	connectionCallback_(shared_from_this());

	loop_->removeChannel(channel_.get());
}















