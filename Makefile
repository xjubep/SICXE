CC=gcc
CFLAGS=-g -Wall
OBJS=main.o shell.o
TARGET=a.out

all: $(TARGET)

clean:
	rm -f *.o
	rm -f $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

main.o: shell.h shell.c main.c
shell.o: shell.h shell.c