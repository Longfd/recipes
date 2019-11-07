#include "Connector.h"

#include "Channel.h"
#include "EventLoop.h"
#include "SocketOps.h"

#include <errno.h>
#include <string.h>
#include <iostream>
#include <assert.h>
#include <algorithm>

using std::cout;
using std::endl;

const int Connector::kMaxRetryDelayMs;

__thread char t_errnobuf[512];

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
	: loop_(loop),
	  serverAddr_(serverAddr),
	  connect_(false),
	  state_(kDisconnected),
	  retryDelayMs_(kInitRetryDelayMs)
{
	cout << "Connector::Connector() " << this << endl;
}

Connector::~Connector()
{
	cout << "Connector::~Connector() " << this << endl;
	loop_->cancel(timerId_);
	assert(!channel_);
}

void Connector::start()
{
	connect_ = true;
	loop_->runInLoop(std::bind(&Connector::startInLoop, this)); // FIXME unsafe
}

void Connector::startInLoop()
{
	loop_->assertInLoopThread();
	assert(state_ == kDisconnected);
	if (connect_)
	{
		connect();
	}
	else
	{
		cout << "Connector::startInLoop(): do not connect\n";
	}
}

void Connector::connect()
{
	int sockfd = sockets::createNonblockingOrDie();
	int ret = sockets::connect(sockfd, serverAddr_.getSockAddrInet());
	int savedErrno = (ret == 0) ? 0 : errno;

	switch (savedErrno)
	{
		case 0:
		case EINPROGRESS:
		case EINTR:
		case EISCONN:
			connecting(sockfd);
			break;

		case EAGAIN:
		case EADDRINUSE:
		case EADDRNOTAVAIL:
		case ECONNREFUSED:
		case ENETUNREACH:
			retry(sockfd);
			break;

		 case EACCES:
		 case EPERM:
		 case EAFNOSUPPORT:
		 case EALREADY:
		 case EBADF:
		 case EFAULT:
		 case ENOTSOCK:
		 	cout << "connect error in Connector::startInLoop " << savedErrno << endl;
			sockets::close(sockfd);
			break;

		default:
			cout << "Unexpected error in Connector::startInLoop " << savedErrno << endl;
			sockets::close(sockfd);
			break;
	}
}

void Connector::connecting(int sockfd)
{
	setState(kConnecting);
	assert(!channel_);
	channel_.reset(new Channel(loop_, sockfd));
	channel_->setWriteCallback(std::bind(&Connector::handleWrite, this));
	channel_->setErrorCallback(std::bind(&Connector::handleError, this));
	channel_->enableWriting();
}

void Connector::retry(int sockfd)
{
	sockets::close(sockfd);
	setState(kDisconnected);
	if (connect_)
	{
		cout << "Connector::retry() connecting to " 
			 << serverAddr_.toHostPort()
			 << " in " << retryDelayMs_ << "milliseconds.\n";
		timerId_ = loop_->runAfter(retryDelayMs_/1000.0, 
								   std::bind(&Connector::startInLoop, this));	
		retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
	}
	else
	{
		cout << "Connector::retry(): do not connect\n";
	}
}

void Connector::restart()
{
	loop_->assertInLoopThread();
	setState(kDisconnected);
	retryDelayMs_ = kInitRetryDelayMs;
	connect_ = true;
	startInLoop();
}

void Connector::stop()
{
	connect_ = false;
	loop_->cancel(timerId_);
}

int Connector::removeAndResetChannel()
{
	channel_->disableAll();
	loop_->removeChannel(channel_.get());
	int sockfd = channel_->fd();
	// can't reset channel here, because we are inside Channel::handleEvent
	loop_->queueInLoop(std::bind(&Connector::resetChannel, this)); // FIXME unsafe
	return sockfd;
}

void Connector::resetChannel()
{
	channel_.reset();
}

void Connector::handleWrite()
{
	cout << "Connector::handleWrite(): state_:" << state_ << endl;

	if (state_ == kConnecting)
	{
		int sockfd = removeAndResetChannel();
		int err = sockets::getSocketError(sockfd);
		if (err)
		{
			cout << "Connector::handleWrite(): SO-ERROR = " << err 
				 << " :" << strerror_r(err, t_errnobuf, sizeof(t_errnobuf)) << endl;
		}
		else if (sockets::isSelfConnect(sockfd))
		{
			cout << "Connector::handleWrite(): Self connect\n";
			retry(sockfd);
		}
		else
		{
			setState(kConnected);
			if (connect_)
				newConnectionCallback_(sockfd);
			else
				sockets::close(sockfd);
		}
	}
	else
	{
		// what happend?
		assert(state_ == kDisconnected);
	}
}

void Connector::handleError()
{
	cout << "Connector::handleError()\n";
	assert(state_ == kConnecting);

	int sockfd = removeAndResetChannel();
	int err = sockets::getSocketError(sockfd);
	cout << "SO_ERROR = " << err << endl;
	retry(sockfd);
}




