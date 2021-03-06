#include "unp.h" 

void str_cli(FILE *fp, int sockfd)
{
	int maxfd1, n;
	fd_set rset;
	char sendline[MAXLINE], recvline[MAXLINE];

	int stdinof = 0;
	FD_ZERO(&rset);
	while (1) {
		maxfd1 = MAX(fileno(fp), sockfd) + 1;
		if(stdinof == 0)
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);

		if ( select(maxfd1, &rset, NULL, NULL, NULL) < 0) 
			err_quit("[str_cli]select error");

		if(FD_ISSET(sockfd, &rset)) {
			if ( (n = Read(sockfd, recvline, MAXLINE)) == 0) {
				if(stdinof == 1)
					return;
				else
					err_quit("str_cli: server terminate prematurely");
			}
			Write(fileno(stdout), recvline, n);
		}
		if(FD_ISSET(fileno(fp), &rset)) {
			if ( (n = Read(fileno(fp), sendline, MAXLINE)) == 0) {
				// read EOF client close wirte
				stdinof = 1;
				Shutdown(sockfd, SHUT_WR); // send FIN
				FD_CLR(fileno(fp), &rset);
			}
			else 
				Writen(sockfd, sendline, n);
		}
	}
}

