CC=gcc
CFLAGS=-g -Wall
OBJS=main.o shell.o list.o opcode.o
TARGET=a.out

all: $(TARGET)

clean:
	rm -f *.o
	rm -f $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

main.o: main.c shell.h shell.c list.h list.c opcode.h opcode.c
shell.o: shell.h shell.c
list.o: list.h list.c
opcode.o: opcode.h opcode.c