CC=gcc
CFLAGS=-Wall -W
SOURCES=main.c shell.c list.c opcode.c memory.c assemble.c symbol.c
TARGET=a.out

all: $(TARGET)

clean:
	rm -f $(TARGET) *.lst *.obj

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)