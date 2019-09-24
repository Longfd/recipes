OBJS = str_cli.o str_clinonb.o str_echo.o \
	   writen.o error.o signal.o \
	   wrapsock.o wrapunix.o

all: unplib clean

unplib: ${OBJS}
	ar rcs ../libunp.a $^ 

clean:
	rm -rf *.o
