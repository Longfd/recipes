#include "unp.h"

int Fcntl(int fd, int cmd, int arg)
{
	int	n;

	if ( (n = fcntl(fd, cmd, arg)) == -1)
		err_quit("fcntl error");
	return(n);
}

char * gf_time(void)
{
	struct timeval	tv;
	time_t			t;
	static char str[32]; // static for return str variable
	char buf[32];

	if (gettimeofday(&tv, NULL) < 0)
		err_quit("gettimeofday error");

	t = tv.tv_sec;	/* POSIX says tv.tv_sec is time_t; some BSDs don't agree. */
	ctime_r(&t, buf);
	/* Fri Sep 13 00:00:00 1986\n\0 */
	strcpy(str, &buf[11]);

	/* 00:00:00.123456 */
	snprintf(str+8, sizeof(str)-8, ".%06ld", tv.tv_usec);

	return(str);
}


