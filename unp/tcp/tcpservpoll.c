#include "unp.h"


int main(int argc, char **argv)
{
	int i, maxi, listenfd, connfd, sockfd;
	const int openmax = open_max();
	struct sockaddr_in servaddr, cliaddr;
	struct pollfd client[openmax];	
	char buf[MAXLINE], ipaddr[16];
	socklen_t clilen = sizeof(cliaddr);

	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_quit("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	if ( bind(listenfd, (SA*)&servaddr, sizeof(servaddr)) == -1)
		err_quit("bind error");

	if ( listen(listenfd, LISTENQ) == -1)
		err_quit("listen error");

	// pollfd initialize
	for (i = 0; i < openmax; ++i)
		client[i].fd = -1;
	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;

	maxi = 0;
	while (1) {
		int nready = Poll(client, maxi + 1, INFTIM);

		if (client[0].revents & POLLRDNORM) {
			// new connection
			if ( (connfd = accept(listenfd, (SA*)&cliaddr, &clilen)) == -1)
				err_quit("accept error");
	
			printf("new connection: [%s]:[%d]\n",
					inet_ntop(AF_INET, (void*)&cliaddr.sin_addr, ipaddr, sizeof(ipaddr)),
					ntohs(cliaddr.sin_port));

			for(i = 0; i < openmax; ++i) {
				if(client[i].fd == -1) {
					client[i].fd = connfd;
					client[i].events = POLLRDNORM;
					break;
				}
			}
			if(i == openmax)
				err_quit("too many clients");
			if(i > maxi)
				maxi = i;
			if(--nready <= 0)
				continue;
		}

		for (i = 0; i <= maxi; ++i) {
			if( (sockfd = client[i].fd) == -1)
				continue;

			if(client[i].revents & (POLLRDNORM | POLLERR)) {
				int n = read(sockfd, buf, MAXLINE);
				if(n < 0) {
					if(n == ECONNRESET) {
						printf("client aborted connection\n");
						close(sockfd);
						client[i].fd = -1;
					}
					else
						err_quit("[tcpservpoll]:read error");
				}
				else if(n == 0) {
					printf("client closed connection\n");
					close(sockfd);
					client[i].fd = -1;
				}
				else
					Write(client[i].fd, buf, n);

				if(--nready <= 0)
					break;
			}
		}
	}
}

