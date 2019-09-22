PROGS =	server servselect servpoll \
		client
CC = gcc -std=c99
CFLAGS = -I../lib -g -O2 -D_REENTRANT -Wall
LIBS = ../libunp.a -lpthread

all: ${PROGS} 

server: tcpserv.o 
	${CC} ${CFLAGS} -o $@ tcpserv.o ${LIBS}

servselect: tcpservselect.o 
	${CC} ${CFLAGS} -o $@ tcpservselect.o ${LIBS}

servpoll: tcpservpoll.o 
	${CC} ${CFLAGS} -o $@ tcpservpoll.o ${LIBS}

client: tcpcli.o  
	${CC} ${CFLAGS} -o $@ tcpcli.o ${LIBS}

clean:
	rm -rf *.o ${PROGS}

