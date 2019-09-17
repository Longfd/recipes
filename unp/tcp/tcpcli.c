#include "unp.h"

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 2) {
		printf("usage: %s <IP address>", argv[0]);
		exit(-1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	if ( (inet_pton(AF_INET, argv[1], (void*)&servaddr.sin_addr)) <= 0) {
		perror("inet_pton error");
		exit(-1);
	}

	if ( connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect error");
		exit(-1);
	}

	str_cli(stdin, sockfd);
	exit(0);
}

