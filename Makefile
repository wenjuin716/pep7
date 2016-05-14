DIR=$(shell pwd)

CC=gcc
CFLAGS=
COMMON_OBJS=
OBJS=
EXE=pep7

all: main.o

main.o: $(COMMON_OBJS) $(OBJS)
	$(CC) $(COMMON_OBJS) $(OBJS) $(CFLAGS) -o $(EXE) main.c

clean:
	rm -rf *.o $(OBJS) $(COMMON_OBJS) $(EXE)
