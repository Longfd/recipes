#include "unp.h"


int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	int	n;

	if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
		err_quit("select error");
	return(n);		/* can return 0 on timeout */
}

int Poll(struct pollfd* fdarray, unsigned long nfds, int timeout)
{
	int n;

	if ( (n = poll(fdarray, nfds, timeout)) < 0)
		err_quit("poll error");

	return n;
}

long open_max(void)
{
	int openmax;

	if (openmax == 0) {     /* first time through */
		errno = 0;
		if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
			if (errno == 0)
				openmax = OPEN_MAX_GUESS;   /* it's indeterminate */
			else
				err_quit("sysconf error for _SC_OPEN_MAX");
		}
	}

	return(openmax);
}

void Shutdown(int fd, int how)
{
	if (shutdown(fd, how) < 0)
		err_quit("shutdown error");
}


