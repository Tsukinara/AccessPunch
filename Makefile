CC = gcc
CFLAGS = -std=c99 -Wall -g -O0 -Wwrite-strings -Wshadow \
	-fstack-protector-all
LDFLAGS = -lwiringPi
PROGS = access_punch
SANDWICH = me a sandwich

.PHONY: all clean $(SANDWICH)

all: $(PROGS)
clean:
	rm -f *.o $(PROGS) *~

#reference to XKCD #149, nothing to do with program
me:
	@true
a:
	@true
sandwich:
	@[ -w /etc/shadow ] && echo "Okay." || echo "What? Make it yourself."

#linking .o files
access_punch: main.o swipe_read.o
	$(CC) $(LDFLAGS) -o access_punch main.o swipe_read.o

#creating .o files
main.o: main.c swipe_read.h
	$(CC) $(CFLAGS) -c main.c
swipe_read.o: swipe_read.c swipe_read.h
	$(CC) $(CFLAGS) -c swipe_read.c
