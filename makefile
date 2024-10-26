# Variables
CC = gcc
CFLAGS = -Wall -std=c99
LIBS = -lSDL2 -lSDL2_image -lSDL2_gfx -lm
TARGET = all
SRCS = main.c

# Default target
all:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

# Clean up
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean

