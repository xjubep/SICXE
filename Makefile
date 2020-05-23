CC=gcc
CFLAGS=-Wall -W
SOURCES=main.c shell.c list.c opcode.c memory.c assemble.c symbol.c load.c run.c
TARGET=a.out

all: $(TARGET)

clean:
	rm -f $(TARGET) *.lst

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)