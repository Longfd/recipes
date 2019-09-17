#include "unp.h"

int main(int argc, char **argv)
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	if( (listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("sockect error");
		exit(-1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	if( bind(listenfd, (SA*)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind error");
		exit(-1);
	}

	if(listen(listenfd, LISTENQ) < 0) {
		perror("listen error");
		exit(-1);
	}

	for (;;) {
		clilen = sizeof(cliaddr);
		if( (connfd = accept(listenfd, (SA*)&cliaddr, &clilen)) < 0) {
			perror("accept error");
			exit(-1);
		}

		if( (childpid = fork()) == 0) { // child process
			if(close(listenfd) < 0) {
				perror("close error");
				exit(-1);
			}

			str_echo(connfd);
			exit(0);
		}
		
		if(close(connfd) < 0) {
			perror("close error");
			exit(-1);
		}
	}
}

