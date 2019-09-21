#include "unp.h"


int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	int	n;

	if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
		err_quit("select error");
	return(n);		/* can return 0 on timeout */
}

