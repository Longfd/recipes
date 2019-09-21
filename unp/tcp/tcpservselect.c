#include "unp.h"

int main(int argc, char **argv)
{
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t n;
	fd_set rset, allset;
	char buf[MAXLINE], clientip[16];
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	if( (listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
		err_quit("sockect error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	if( bind(listenfd, (SA*)&servaddr, sizeof(servaddr)) < 0) 
		err_quit("bind error");

	if(listen(listenfd, LISTENQ) < 0) 
		err_quit("listen error");

	maxfd = listenfd; // initialize
	maxi = -1; // index into client[] array
	for(i = 0; i < FD_SETSIZE; ++i) 
		client[i] = -1;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	for (;;) {
		rset = allset;
		nready = Select(maxfd+1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(listenfd, &rset)) {
			clilen = sizeof(cliaddr);
			if( (connfd = accept(listenfd, (SA*)&cliaddr, &clilen)) < 0) 
				err_quit("accept error");

			printf("new client: %s, port: %d\n",
					inet_ntop(AF_INET, &cliaddr.sin_addr, clientip, sizeof(clientip)),
					ntohs(cliaddr.sin_port));

			for(i = 0; i < FD_SETSIZE; ++i) {
				if(client[i] < 0) {
					client[i] = connfd;
					break;
				}
			}
			if(i == FD_SETSIZE)
				err_quit("too many clients");

			FD_SET(connfd, &allset); // connfd will add to rset in next loop
			if(maxfd < connfd)
				maxfd = connfd; // for select
			if(maxi < i)
				maxi = i; // max index in client[] array

			if(--nready <= 0) // no more readable descriptor
				continue;
		} 

		for (i = 0; i <= maxi; ++i) {
			if( (sockfd = client[i]) < 0 )
				continue;
			if(FD_ISSET(sockfd, &rset)) {
				if( (n = Read(sockfd, buf, MAXLINE)) == 0) {
					// peer closed
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}
				else
					Write(sockfd, buf, n);

				if(--nready <= 0)
					break;
			}
		}
	}
}

