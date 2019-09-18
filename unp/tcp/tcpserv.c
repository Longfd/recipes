#include "unp.h"

int main(int argc, char **argv)
{
	int listenfd, connfd;
	pid_t childpid;
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

	for (;;) {
		clilen = sizeof(cliaddr);
		if( (connfd = accept(listenfd, (SA*)&cliaddr, &clilen)) < 0) 
			err_quit("accept error");

		if( (childpid = fork()) == 0) { // child process
			if(close(listenfd) < 0) 
				err_quit("close error");

			str_echo(connfd);
			exit(0);
		}
		
		if(close(connfd) < 0) 
			err_quit("close error");
	}
}

