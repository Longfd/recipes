PROGS = test01 test02 test03
CC = g++ -std=c++11
CFLAGS = -I./ -D_REENTRANT -D__STDC_FORMAT_MACROS -Wall
LIBFLAG = -lpthread
BASE_SRC = ../base/CurrentThread.cpp ../base/TimeStamp.cpp ../base/Thread.cc
NET_SRC = EventLoop.cpp Channel.cc Poller.cc EventLoopThread.cc TimerQueue.cc Timer.cc

ifeq ($(BUILD), release)   
# "Release" build - optimization, and no debug symbols
# debugging symbols stripped (-s) and assertions disabled (-DNDEBUG)
CFLAGS += -O2 -s -DNDEBUG
else
#"Debug" build - no optimization, and debugging symbols
CFLAGS += -O0 -g
endif


all: $(PROGS) 

release:
	make "BUILD = release"

test01: test01.cc $(BASE_SRC) $(NET_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBFLAG)

test02: test02.cc $(BASE_SRC) $(NET_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBFLAG)

test03: test03.cc $(BASE_SRC) $(NET_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBFLAG)

clean:
	rm -rf *.o $(PROGS)

