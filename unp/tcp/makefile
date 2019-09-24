PROGS =	server servselect servpoll \
		client
CC = gcc -std=c99
CFLAGS = -I../lib -g -O2 -D_REENTRANT -Wall
LIBFLAG = ../libunp.a -lpthread
LIBS = ../libunp.a

all: ${PROGS} 

server: tcpserv.o ${LIBS}
	${CC} ${CFLAGS} -o $@ tcpserv.o ${LIBFLAG}

servselect: tcpservselect.o ${LIBS}
	${CC} ${CFLAGS} -o $@ tcpservselect.o ${LIBFLAG}

servpoll: tcpservpoll.o ${LIBS}
	${CC} ${CFLAGS} -o $@ tcpservpoll.o ${LIBFLAG}

client: tcpcli.o ${LIBS}
	${CC} ${CFLAGS} -o $@ tcpcli.o ${LIBFLAG}

clean:
	rm -rf *.o ${PROGS}

