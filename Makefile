CC=gcc
LD=gcc

CFLAGS=-g -Wall
LDFLAGS=-g

TARGET=testgl.exe

SRCS=$(wildcard src/*.c)

LIBS=-lopengl32 -lgdi32 -luser32 -lm

OBJS=$(patsubst src/%.c,build/%.o,$(SRCS))

.PHONY: all clean

all: $(TARGET)

build/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
	
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@
	
clean:
	rm -rf build/ $(TARGET)
	