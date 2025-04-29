.SILENT:

CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
LDFLAGS = -lm
TARGET = pc
SRC = main.c queue.c
OBJ = main.o queue.o
DEPS = work.h queue.h

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
