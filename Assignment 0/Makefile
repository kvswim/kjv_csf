CFLAGS=-std=c11 -Wall -Wextra -Wpedantic -O2
ALL=wc hex

all: $(ALL)

wc: wc.o

wc.o: wc.c
hex.o: hex.c

hex: hex.o

clean:
	rm -f *.o $(ALL)
