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
	peerAddr_(peerAddr),
	highWaterMark_(64 * 1024 * 1024)
{
	std::cout << "TcpConnection::ctor[" << name << "] at" << this 
		<< " fd = " << sockfd << std::endl;
	channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, _1));
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
		messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
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
	loop_->assertInLoopThread();
	if (channel_->isWriting()) 
	{
		ssize_t n = ::write(channel_->fd(),
				outputBuffer_.peek(),
				outputBuffer_.readableBytes());
		if (n > 0) 
		{
			outputBuffer_.retrieve(n);
			if (outputBuffer_.readableBytes() == 0) 
			{
				channel_->disableWriting();

				if (writeCompleteCallback_) {
					loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
				}

				if (state_ == kDisconnecting) {
					shutdownInLoop();
				}
			} 
			else {
				std::cout << "I am going to write more data\n";
			}
		} 
		else {
			std::cout << "TcpConnection::handleWrite\n";
			exit(EXIT_FAILURE);
		}
	} 
	else 
	{
		std::cout << "Connection is down, no more writing\n";
	}
}

void TcpConnection::handleClose()
{
	loop_->assertInLoopThread();
	std::cout << "TcpConnection::handleClose state = " << state_ << std::endl;
	assert(state_ == kConnected || state_ == kDisconnecting);
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
	assert(state_ == kConnected || state_ == kDisconnecting);
	setState(kDisconnected);
	channel_->disableAll();
	connectionCallback_(shared_from_this());

	loop_->removeChannel(channel_.get());
}

void TcpConnection::shutdown()
{
	if (state_ == kConnected)
	{
		setState(kDisconnecting);
		loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
	}
}

void TcpConnection::shutdownInLoop()
{
	loop_->assertInLoopThread();
	if (!channel_->isWriting())
	{
		socket_->shutdownWrite();
	}
}

void TcpConnection::send(const std::string& message)
{
	if (state_ != kConnected)
	{
		return;
	}

	if (loop_->isInLoopThread())
	{
		sendInLoop(message);
	}
	else
	{
		loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message));
	}
}

void TcpConnection::sendInLoop(const std::string& message)
{
	loop_->assertInLoopThread();
	ssize_t nwrote = 0;
	size_t remaining = message.size();
	bool faultError = false;

	// if no thing in output queue, try writing directly
	if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
		nwrote = ::write(channel_->fd(), message.data(), message.size());
		if (nwrote >= 0) {
			remaining -= nwrote;

			if (remaining == 0 && writeCompleteCallback_) {
				loop_->runInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
			}

			if (static_cast<size_t>(nwrote) < message.size()) {
				std::cout << "I am going to write more data\n";
			}
		} 
		else { // nwrote < 0
			nwrote = 0;
			std::cout << "TcpConnection::sendInLoop\n";
			if (errno != EWOULDBLOCK) 
			{
				if (errno == EPIPE || errno == ECONNRESET) 
					faultError = true;	
				else
					exit(EXIT_FAILURE);
			}
		}
	}

	assert(remaining <= message.size());

	if (!faultError && remaining > 0)
	{
		size_t oldLen = outputBuffer_.readableBytes();
		if (oldLen + remaining >= highWaterMark_ &&
			oldLen < highWaterMark_/* callback while overload at the first time*/ &&
			highWaterMark_)
		{
			loop_->queueInLoop(std::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
		}

		outputBuffer_.append(message.data()+nwrote, message.size()-nwrote);
		if (!channel_->isWriting()) {
			channel_->enableWriting();
		}
	}
}


void TcpConnection::setTcpNoDelay(bool on)
{
	socket_->setTcpNoDelay(on);
}








