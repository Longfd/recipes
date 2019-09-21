PROGS =	server servselect\
		client
CC = gcc
CFLAGS = -I../lib -g -O2 -D_REENTRANT -Wall
LIBS = ../libunp.a -lpthread

all: ${PROGS} clean

server: tcpserv.o 
	${CC} ${CFLAGS} -o $@ tcpserv.o ${LIBS}

servselect: tcpservselect.o 
	${CC} ${CFLAGS} -o $@ tcpservselect.o ${LIBS}

client: tcpcli.o  
	${CC} ${CFLAGS} -o $@ tcpcli.o ${LIBS}

clean:
	rm -rf *.o
clear:
	rm -rf ${PROGS}

