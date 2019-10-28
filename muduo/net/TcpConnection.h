#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "Callbacks.h"
#include "InetAddress.h"
#include "../base/Noncopyable.h"

// #include <any> // c++17 
#include <memory>


class Channel;
class EventLoop;
class Socket;

class TcpConnection : Noncopyable
					  public std::enable_shared_from_this<TcpConnection>
{
public:
	TcpConnection(EventLoop* loop,
				  const std::string name,
				  int sockfd,
				  const InetAddress& localAddr,
				  const InetAddress& peerAddr);
	~TcpConnection();

	EventLoop* getLoop() const { return loop_; }
	const std::string& name() const { return name_; }
	const InetAddress& localAddress() { return localAddr_; }
	const InetAddress& peerAddress() { return peerAddr_; }
	bool connected() const { return state_ == kConnected; }

	void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
	void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }

	void connectEstablished();

private:
	enum StateE { kConnecting, kConnected};

	void setState(StateE s) { state_ = s; }
	void handleRead();

	EventLoop* loop_;
	std::string name_;
	StateE state_;
	std::unique_ptr<Socket> socket_;
	std::unique_ptr<Channel> channel_;
	InetAddress localAddr_;
	InetAddress peerAddr_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
};

#endif // TCPCONNECTION_H






















