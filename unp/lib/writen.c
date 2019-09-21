#include "unp.h"


int Read(int fd, void* buf, size_t nbytes)
{
	size_t n;

	if( (n= read(fd, buf, nbytes)) < 0)
		err_quit("read error");

	return n;
}

void Write(int fd, void *ptr, size_t nbytes)
{
	if (write(fd, ptr, nbytes) != nbytes)
		err_quit("write error");
}

/* Write "n" bytes to a descriptor. */
ssize_t	writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}
/* end writen */
void Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes) {
		perror("writen error");
		exit(-1);
	}
}


