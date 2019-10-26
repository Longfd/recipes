#ifndef SOCKET_H
#define SOCKET_H

#include "../base/Noncopyable.h"


class InetAddress;
class Socket : Noncopyable
{
public:
	explicit Socket(int sockfd)
		: sockfd_(sockfd)
	{
	}

	~Socket();

	int fd() const { return sockfd_; }

	// abort if address in use
	void bindAddress(const InetAddress& localaddr);

	// abort if address in unsigned
	void listen();

	int accept(InetAddress* peeraddr);

	void setReuseAddr(bool on);

private:
	const int sockfd_;
};

#endif















