#include "unp.h"

void str_clinonb(FILE* fp, int sockfd)
{
	int maxfd1, val, stdineof;
	ssize_t n, nwritten;
	fd_set rset, wset;
	char to[MAXLINE], fr[MAXLINE];
	char *toiptr, *tooptr, *friptr, *froptr;

	val = Fcntl(sockfd, F_GETFL, 0);	
	Fcntl(sockfd, F_SETFL, val|O_NONBLOCK);

	val = Fcntl(STDIN_FILENO, F_GETFL, 0);
	Fcntl(STDIN_FILENO, F_SETFL, val|O_NONBLOCK);

	val = Fcntl(STDOUT_FILENO, F_GETFL, 0);
	Fcntl(STDOUT_FILENO, F_SETFL, val|O_NONBLOCK);

	toiptr = tooptr = to;
	friptr = froptr = fr;
	stdineof = 0;

	maxfd1 = MAX(MAX(STDIN_FILENO, STDOUT_FILENO), sockfd);
	for (;;) {
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		if(stdineof == 0 && toiptr < &to[MAXLINE])
			FD_SET(STDIN_FILENO, &rset);
		if(friptr < &fr[MAXLINE])
			FD_SET(sockfd, &rset);
		if(tooptr != toiptr)
			FD_SET(sockfd, &wset);
		if(froptr != friptr)
			FD_SET(STDOUT_FILENO, &wset);

		Select(maxfd1 + 1, &rset, &wset, NULL, NULL);
		if(FD_ISSET(STDIN_FILENO, &rset)) {
			if( (n = read(STDIN_FILENO, toiptr, &to[MAXLINE] - toiptr)) < 0) {
				if(errno != EWOULDBLOCK)
					err_quit("read error on stdin");
			}
			else if(n == 0) {
				fprintf(stderr, "%s: EOF on stdin\n", gf_time());

				stdineof = 1;
				if(toiptr == tooptr) // if array to[] has no data, send FIN
					Shutdown(sockfd, SHUT_WR);
			}
			else {
				fprintf(stderr, "%s: read %d bytes from stdin\n", gf_time(), n);

				toiptr += n;
				FD_SET(sockfd, &wset); /* try and write to socket below */
			}
		}

		if(FD_ISSET(sockfd, &rset)) {
			if ( (n = read(sockfd, friptr, &fr[MAXLINE] - friptr)) < 0) {
				if(errno != EWOULDBLOCK)
					err_quit("read error on socket");
			}
			else if(n == 0) {
				fprintf(stderr, "%s: EOF on socket\n", gf_time());

				if(stdineof)
					return; // recv FIN, client return normally
				else
					err_quit("str_cli: server terminated prematurely");
			}
			else {
				fprintf(stderr, "%s: read %d bytes from socket\n", gf_time(), n);

				friptr += n;
				FD_SET(STDOUT_FILENO, &wset); // try and write below
			}
		}

		// if the space of tcp send buffer larger than tcp low warter mark
		// then the sockfd will be set in wset
		// so we need to check n, if there  is any data to be sent
		if(FD_ISSET(STDOUT_FILENO, &wset) && ( (n = friptr - froptr) > 0)) {
			if( (nwritten = write(STDOUT_FILENO, froptr, n)) == -1 ) {
				if(errno != EWOULDBLOCK)
					err_quit("write error to stdout");
			}
			else {
				fprintf(stderr, "%s: wrote %d bytes to socket\n", gf_time(), nwritten);

				froptr += nwritten;
				if(froptr == friptr)
					friptr = froptr = fr;
			}
		}

		if(FD_ISSET(sockfd, &wset) && ( (n = toiptr - tooptr) > 0)) {
			if( (nwritten = write(sockfd, tooptr, n)) == -1) {
				if(errno == EWOULDBLOCK)
					err_quit("write error to socket");
			}
			else {
				fprintf(stderr, "%s: wrote %d bytes to socket\n", gf_time(), nwritten);

				tooptr += nwritten;
				if(toiptr == tooptr) {
					toiptr = tooptr = to;
					if(stdineof)
						Shutdown(sockfd, SHUT_WR); // send FIN after data in array to[] has been sent
				}
			}
		}
	}
}

