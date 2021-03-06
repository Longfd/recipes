#ifndef	__unp_h
#define	__unp_h

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <poll.h>
#include <fcntl.h>


#define	SA	struct sockaddr
#define SERV_PORT 6000
#define	LISTENQ		1024	/* 2nd argument to listen() */
#define	MAXLINE		4096	/* max text line length */

#if	defined(SIG_IGN) && !defined(SIG_ERR)
#define	SIG_ERR	((Sigfunc *)-1)
#endif
typedef	void	Sigfunc(int);	/* for signal handlers */

#ifndef INFTIM
#define INFTIM          (-1)    /* infinite poll timeout */
#endif

#define	MIN(a,b)	((a) < (b) ? (a) : (b))
#define	MAX(a,b)	((a) > (b) ? (a) : (b))

/* for poll openmax */
#define OPEN_MAX_GUESS  256
long open_max(void);

/*basic function*/
void err_quit(const char* fmt, ...);

int Read(int fd, void* buf, size_t nbytes);
void Write(int fd, void *ptr, size_t nbytes);
void Writen(int fd, void *ptr, size_t nbytes);

Sigfunc* Signal(int signo, Sigfunc *func);	/* for our signal() function */

void str_echo(int sockfd);
void str_cli(FILE *fp, int sockfd);
void str_clinonb(FILE* fp, int sockfd);

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int Poll(struct pollfd* fdarray, unsigned long nfds, int timeout);
void Shutdown(int fd, int how);

int Fcntl(int fd, int cmd, int arg);

char* gf_time(void);

#endif //__unp_h

