CC = gcc
FLAGS = -Wall -Werror -Wextra -pedantic -g
TARGETS = Keylogger

.PHONY: all clean

all: $(TARGETS)

Keylogger: Keylogger.c Keylogger.h
	$(CC) $(FLAGS) -c $^
	$(CC) $(FLAGS) -o $@ $@.o


clean:
	rm -f *.o *.h.gch $(TARGETS)