#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "../base/Noncopyable.h"
#include "Channel.h"
#include "Socket.h"

#include <functional>


class EventLoop;
class InetAddress;

class Acceptor : Noncopyable
{
public:
	typedef std::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

	Acceptor(EventLoop* loop, const InetAddress& listenAddr);
	~Acceptor() {}

	void setNewConnectionCallback(const NewConnectionCallback& cb)
	{ newConnectionCallback_ = cb; }

	bool listening() { return listening_; }

	void listen();

private:
	void handleRead();

	EventLoop* loop_;
	Socket acceptSocket_;
	Channel acceptChannel_;
	NewConnectionCallback newConnectionCallback_;
	bool listening_;
};


#endif // ACCEPTOR_H














