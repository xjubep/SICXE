CC=gcc
CFLAGS=-Wall -W
SOURCES=main.c shell.c list.c opcode.c memory.c assemble.c
TARGET=a.out

all: $(TARGET)

clean:
	rm -f $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)