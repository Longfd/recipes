#ifndef	__unp_h
#define	__unp_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#define	SA	struct sockaddr
#define SERV_PORT 6000
#define	LISTENQ		1024	/* 2nd argument to listen() */
#define	MAXLINE		4096	/* max text line length */

#if	defined(SIG_IGN) && !defined(SIG_ERR)
#define	SIG_ERR	((Sigfunc *)-1)
#endif
typedef	void	Sigfunc(int);	/* for signal handlers */

void Writen(int fd, void *ptr, size_t nbytes);

ssize_t Readline(int fd, void *ptr, size_t maxlen);

void str_echo(int sockfd);

void str_cli(FILE *fp, int sockfd);

void Fputs(const char *ptr, FILE *stream);

char* Fgets(char *ptr, int n, FILE *stream);

void err_quit(const char* fmt, ...);

Sigfunc* Signal(int signo, Sigfunc *func);	/* for our signal() function */

#endif //__unp_h

