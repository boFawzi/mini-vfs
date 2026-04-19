CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -Wpedantic -g -I./include
SRCS    = src/main.c src/fs_core.c src/parser.c src/utils.c
OBJS    = $(SRCS:.c=.o)
TARGET  = vfs

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
