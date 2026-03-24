CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O0

TARGETS = controller implant

all: clean $(TARGETS)

controller: controller.c protocol.c
	$(CC) $(CFLAGS) controller.c protocol.c -o controller

implant: implant.c protocol.c
	$(CC) $(CFLAGS) implant.c protocol.c -o implant

clean:
	rm -f $(TARGETS)