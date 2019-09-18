#include "unp.h"
#include <stdarg.h> /* ISO C variable arguments */

/* Print message and terminate */
void err_quit(const char* fmt, ...)
{
	char buf[MAXLINE + 1] = {0};
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, MAXLINE, fmt, ap);
	perror(buf);
	va_end(ap);

	exit(1);
}

