#include "unp.h"

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 2) 
		err_quit("usage: %s <IP address>", argv[0]);

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	if ( (inet_pton(AF_INET, argv[1], (void*)&servaddr.sin_addr)) <= 0) 
		err_quit("inet_pton error");

	if ( connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0) 
		err_quit("connect error");

	str_clinonb(stdin, sockfd);
	exit(0);
}

