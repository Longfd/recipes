PROGS =	tcpserv \
		tcpcli
CC = gcc
CFLAGS = -I../lib -g -O2 -D_REENTRANT -Wall
LIBS = ../libunp.a -lpthread

all: ${PROGS} clean

tcpserv: tcpserv.o 
	${CC} ${CFLAGS} -o $@ tcpserv.o ${LIBS}

tcpcli: tcpcli.o  
	${CC} ${CFLAGS} -o $@ tcpcli.o ${LIBS}

clean:
	rm -rf *.o
clear:
	rm -rf ${PROGS}

