PROGS = test01 test02 test03 test04 test05 \
		test06 test07
CC = g++ -std=c++11
CFLAGS = -I./ -D_REENTRANT -D__STDC_FORMAT_MACROS -Wall
LIBFLAG = -lpthread
BASE_SRC = ../base/CurrentThread.cpp ../base/TimeStamp.cpp ../base/Thread.cc
NET_SRC = EventLoop.cpp Channel.cc Poller.cc EventLoopThread.cc TimerQueue.cc Timer.cc \
		  Acceptor.cc InetAddress.cc Socket.cc SocketOps.cc \
		  TcpServer.cc TcpConnection.cc Buffer.cc EventLoopThreadPool.cc

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

test04: test04.cc $(BASE_SRC) $(NET_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBFLAG)

test05: test05.cc $(BASE_SRC) $(NET_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBFLAG)

test06: test06.cc $(BASE_SRC) $(NET_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBFLAG)

test07: test07.cc $(BASE_SRC) $(NET_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBFLAG)

clean:
	rm -rf *.o $(PROGS)

