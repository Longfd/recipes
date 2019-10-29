#include "SocketOps.h"
#include "../base/IgnoreUnusedWarning.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <strings.h> // bzero
#include <sys/socket.h>
#include <assert.h>
#include <iostream>

#define err_handle(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while(0)


// anonymous namespace for unique funcntion or variable in this file
namespace
{
	typedef struct sockaddr SA;

	template<typename To, typename From>
		inline To implicit_cast(From const &f)
		{
			return f;
		}

	const SA* sockaddr_cast(const struct sockaddr_in* addr)
	{
		return static_cast<const SA*>(implicit_cast<const void*>(addr));
	}

	SA* sockaddr_cast(struct sockaddr_in* addr)
	{
		return static_cast<SA*>(implicit_cast<void*>(addr));
	}

	void setNonBlockAndCloseOnExec(int sockfd)
	{
		// non-block
		int flags = ::fcntl(sockfd, F_GETFL, 0);
		flags |= O_NONBLOCK;
		int ret = ::fcntl(sockfd, F_SETFL, flags);
		assert(ret != -1); IntendToIgnoreThisVariable(ret);

		// close-on-exec
		flags = ::fcntl(sockfd, F_GETFL, 0);
		flags |= FD_CLOEXEC;
		ret = ::fcntl(sockfd, F_SETFL, flags);
		assert(ret != -1); IntendToIgnoreThisVariable(ret);
	}
} // end anonymous namespace

int sockets::createNonblockingOrDie()
{
	int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if (sockfd == -1)
		err_handle("socket() error");

	return sockfd;
}

void sockets::bindOrDie(int sockfd, const sockaddr_in& addr)
{
	int ret = ::bind(sockfd, sockaddr_cast(&addr), sizeof(addr));
	if (ret == -1)
		err_handle("bind() error");
}

void sockets::listenOrDie(int sockfd)
{
	int ret = ::listen(sockfd, SOMAXCONN);
	if (ret == -1)
		err_handle("listen() error");
}

int sockets::accept(int sockfd, struct sockaddr_in* addr)
{
	socklen_t addrlen = sizeof(struct sockaddr_in);

#if VALGRIND
	int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
	setNonBlockAndCloseOnExec(connfd);
#else
	int connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen, 
			SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif

	if (connfd == -1)
	{
		int savedErrno = errno;
		std::cout << "accept error:" << savedErrno << std::endl;

		switch (savedErrno)
		{
			// ignore 
			case EAGAIN:
			case ECONNABORTED:		// A connection has been aborted
			case EINTR:
			case EPROTO:			// Protocol error 
			case EPERM:				// Firewall rules forbid connection
			case EMFILE:			// The per-process limit of open file descriptors has been reached.
				errno = savedErrno;
				break;

				// aborted
			default:
				err_handle("accept() error");
		}
	}

	return connfd;
}

void sockets::close(int sockfd)
{
	int ret = ::close(sockfd);
	if (ret == -1)
		err_handle("close() error");
}

void sockets::toHostPort(char* buf, size_t size, const struct sockaddr_in& addr)
{
	char host[INET_ADDRSTRLEN] = "INVALID";

	::inet_ntop(AF_INET, (void*)&addr.sin_addr, host, sizeof(host));
	uint16_t port = networkToHost16(addr.sin_port);
	snprintf(buf, size, "%s:%u", host, port);
}

void sockets::fromHostPort(const char* ip, uint16_t port, struct sockaddr_in* addr)
{
	addr->sin_family = AF_INET;
	addr->sin_port = hostToNetwork16(port);
	if (::inet_pton(AF_INET, ip, (void*)&addr->sin_addr) <= 0)
		std::cout << "inet_pton() error\n";
}

struct sockaddr_in sockets::getLocalAddr(int sockfd)
{
	struct sockaddr_in localaddr;
	bzero(&localaddr, sizeof localaddr);
	socklen_t addrlen = sizeof(localaddr);
	if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0)
	{
		std::cout << "sockets::getLocalAddr";
	}
	return localaddr;
}















