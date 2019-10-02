PROGS = cond_test 
CC = g++ -std=c++11
CFLAGS = -I./ -g -O2 -D_REENTRANT -Wall
LIBFLAG = -lpthread

all: ${PROGS} 

cond_test: cond_test.o CurrentThread.o
	${CC} ${CFLAGS} -o $@ $^ ${LIBFLAG}

clean:
	rm -rf *.o ${PROGS}

