PROGS = test01 test02
CC = g++ -std=c++11
CFLAGS = -I./ -g -O2 -D_REENTRANT -Wall
LIBFLAG = -lpthread
BASE_SRC = ../base/CurrentThread.cpp ../base/TimeStamp.cpp
NET_SRC = EventLoop.cpp Channel.cc Poller.cc

all: $(PROGS) 

test01: test01.cc $(BASE_SRC) $(NET_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBFLAG)

test02: test02.cc $(BASE_SRC) $(NET_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBFLAG)

clean:
	rm -rf *.o $(PROGS)

