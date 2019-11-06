#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "../base/Noncopyable.h"
#include "Callbacks.h"
#include "InetAddress.h"
#include "Buffer.h"

// #include <any> // c++17 
#include <memory>


class Channel;
class EventLoop;
class Socket;

class TcpConnection : Noncopyable,
					  public std::enable_shared_from_this<TcpConnection>
{
public:
	TcpConnection(EventLoop* loop,
				  const std::string& name,
				  int sockfd,
				  const InetAddress& localAddr,
				  const InetAddress& peerAddr);
	~TcpConnection();

	EventLoop* getLoop() const { return loop_; }
	const std::string& name() const { return name_; }
	const InetAddress& localAddress() { return localAddr_; }
	const InetAddress& peerAddress() { return peerAddr_; }
	bool connected() const { return state_ == kConnected; }

	// Thread safe
	void send(const std::string& message);
	// Thread safe
	void shutdown();

	void setTcpNoDelay(bool on);

	void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
	void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
	// Internal use only
	void setCloseCallback(const CloseCallback& cb) { closeCallback_ = cb; }

	void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }
	void setHighWaterMarkCallback(const HighWaterMarkCallback& cb) { highWaterMarkCallback_ = cb; }

	// called when TcpServer accepts a new connection
	void connectEstablished(); // should be called only once
	// called when TcpServer has removed me from its map
	void connectDestroyed(); // should be called only once

private:
	enum StateE { kConnecting, kConnected, kDisconnecting, kDisconnected};

	void setState(StateE s) { state_ = s; }
	void handleRead(Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();
	void sendInLoop(const std::string& message);
	void shutdownInLoop();

	EventLoop* loop_;
	std::string name_;
	StateE state_;
	std::unique_ptr<Socket> socket_;
	std::unique_ptr<Channel> channel_;
	InetAddress localAddr_;
	InetAddress peerAddr_;
	size_t highWaterMark_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	CloseCallback closeCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	HighWaterMarkCallback highWaterMarkCallback_;
	Buffer inputBuffer_;
	Buffer outputBuffer_;
};

#endif // TCPCONNECTION_H






















