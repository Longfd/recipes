#include "unp.h" 


char* Fgets(char *ptr, int n, FILE *stream)
{
	char *rptr;

	if ( (rptr = fgets(ptr, n, stream)) == NULL && ferror(stream)) {
		perror("fgets error");
		exit(-1);
	}

	return (rptr);
}

void Fputs(const char *ptr, FILE *stream)
{
	if (fputs(ptr, stream) == EOF) {
		perror("fputs error");
		exit(-1);
	}
}


void str_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Writen(sockfd, sendline, strlen(sendline));

		if (Readline(sockfd, recvline, MAXLINE) == 0) {
			perror("Readline error");
			exit(-1);
		}

		Fputs(recvline, stdout);
	}
}

