#include "InetAddress.h"
#include "SocketOps.h"

#include <strings.h> // bzero
#include <assert.h> // static_assert

// /* struct sockaddr_in defined in header: netinet/in.h */
// typedef uint32_t in_addr_t
// struct in_addr {
// 	in_addr_t	s_addr; // 32-bit IPv4 address
// 					  // network byte ordered
// }
//
// struct sockaddr_in {
// 	uint8_t			sin_len; 	  /* length of structure (16) */
// 	sa_family_t		sin_family;   /* AF_INET */
// 	in_port_t 		sin_port; 	  /* 16-bit TCP or UDP port number */
// 	struct in_addr 	sin_addr; 	  /* 32-bit IPv4 address */
//								  /* network byte ordered*/
//	char 			sin_zero[8];  /* unused */
// }


static const in_addr_t kInaddrAny = INADDR_ANY;

static_assert( sizeof(InetAddress) == sizeof(struct sockaddr_in), 
			   "InetAddress is same size as sockaddr_in");

// for server listen
InetAddress::InetAddress(uint16_t port)
{
	bzero(&addr_, sizeof(addr_) );
	addr_.sin_family = AF_INET;
	addr_.sin_addr.s_addr = sockets::hostToNetwork32(kInaddrAny);
	addr_.sin_port = sockets::hostToNetwork16(port);
}

// for client connect
InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
	bzero(&addr_, sizeof(addr_));
	sockets::fromHostPort(ip.c_str(), port, &addr_);
}

std::string InetAddress::toHostPort() const
{
	char buf[32];
	sockets::toHostPort(buf, sizeof(buf), addr_);
	return buf;
}















