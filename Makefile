CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O0 -I include

TARGETS = bin/controller bin/implant

all: clean $(TARGETS)

bin/controller: src/controller.c src/protocol.c | bin
	$(CC) $(CFLAGS) src/controller.c src/protocol.c -o bin/controller

bin/implant: src/implant.c src/protocol.c | bin
	$(CC) $(CFLAGS) src/implant.c src/protocol.c -o bin/implant

bin:
	mkdir -p bin

clean:
	rm -rf bin