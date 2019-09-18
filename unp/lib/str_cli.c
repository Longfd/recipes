#include "unp.h" 


char* Fgets(char *ptr, int n, FILE *stream)
{
	char *rptr;

	if ( (rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
		err_quit("fgets error");

	return (rptr);
}

void Fputs(const char *ptr, FILE *stream)
{
	if (fputs(ptr, stream) == EOF)
		err_quit("fputs error");
}

void str_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Writen(sockfd, sendline, strlen(sendline));

		if (Readline(sockfd, recvline, MAXLINE) == 0) 
			err_quit("str_cli: server terminate prematurely");

		Fputs(recvline, stdout);
	}
}

