#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "Callbacks.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include "../base/Noncopyable.h"

#include <map>

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer : Noncopyable
{
public:
	TcpServer(EventLoop* loop, const InetAddress& listenAddr);
	~TcpServer();

	void start();

	void setConnectionCallback(const ConnectionCallback& cb)
	{
		connectionCallback_ = cb;
	}

	void setMessageCallback(const MessageCallback& cb)
	{
		messageCallback_ = cb;
	}

	void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

	void setThreadNum(int threadNum);

private:
	typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

	void newConnection(int sockfd, const InetAddress& peerAddr);
	void removeConnection(const TcpConnectionPtr& conn);
	void removeConnectionInLoop(const TcpConnectionPtr& conn);

	EventLoop* loop_;
	const std::string name_;
	std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
	std::unique_ptr<EventLoopThreadPool> threadPool_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	bool started_;
	int nextConnId_; // always in loop thread
	ConnectionMap connections_;
};

#include <map>
#include <memory>

#endif // TCPSERVER_H

















