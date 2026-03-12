CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O0

TARGETS = controller implant

all: clean $(TARGETS)

controller: controller.c
	$(CC) $(CFLAGS) controller.c -o controller

implant: implant.c
	$(CC) $(CFLAGS) implant.c -o implant

clean:
	rm -f $(TARGETS)