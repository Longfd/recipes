PROGS =	tcpserv \
		tcpcli

all: ${PROGS} clean

tcpserv: tcpserv.o writen.o str_echo.o
	gcc -o $@ $^

tcpcli: tcpcli.o writen.o str_cli.o readline.o 
	gcc -o $@ $^

clean:
	rm -rf *.o
