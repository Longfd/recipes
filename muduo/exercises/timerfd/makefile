PROG= absolute relative

all: ${PROG}

absolute: timerfd.c
	gcc -o $@ $^

relative: timerfd_monotonic.c
	gcc -o $@ $^

clean:
	rm -rf ${PROG}
