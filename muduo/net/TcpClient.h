#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "../base/Noncopyable.h"
#include "../base/Mutex.h"
#include "TcpConnection.h"
#include "Callbacks.h"


class Connector;
class EventLoop;

typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient : Noncopyable
{
public:
	TcpClient(EventLoop* loop, const InetAddress& serverAddr);
	~TcpClient();

	void connect();
	void disconnect();
	void stop();

	TcpConnectionPtr connection() const
	{
		MutexLockGuard lockGuard(mutex_);
		return connection_;
	}

	bool retry() const;

	void enableRetry() { retry_ = true; }

	void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }

	void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }

	void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

private:
	void newConnection(int sockfd);

	void removeConnection(const TcpConnectionPtr& conn);

	EventLoop* loop_;
	ConnectorPtr connector_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	bool retry_;
	bool connect_;
	// always in loop thread
	int nextConnId_;
	mutable Mutex mutex_;
	TcpConnectionPtr connection_;
};


#endif // TCPCLIENT_H




















