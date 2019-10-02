PROGS = test01 
CC = g++ -std=c++11
CFLAGS = -I./ -g -O2 -D_REENTRANT -Wall
LIBFLAG = -lpthread
BASE_SRC = ../base/CurrentThread.cpp
NET_SRC = EventLoop.cpp

all: $(PROGS) 

test01: test01.cc $(BASE_SRC) $(NET_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBFLAG)

clean:
	rm -rf *.o $(PROGS)

