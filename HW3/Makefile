CFLAGS=-std=c11 -Wall -Wextra -Wpedantic -O2 -g
ALL=sas
LIB= -lm

all: $(ALL)

sas: sas.o
	gcc -o $@ $^ $(LIB)

clean:
	rm -f *.o $(ALL)
