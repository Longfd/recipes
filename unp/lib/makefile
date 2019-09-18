OBJS = readline.o  str_cli.o  str_echo.o \
	   writen.o error.o

all: unplib clean

unplib: ${OBJS}
	ar rcs ../libunp.a $^ 

clean:
	rm -rf *.o
