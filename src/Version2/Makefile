CC = gcc
FLAGS = -Wall -Werror -Wextra -g
TARGETS = Keylogger

.PHONY: all clean

all: $(TARGETS)

Keylogger: keylogger.c keylogger.h
	$(CC) $(FLAGS) -c $^
	$(CC) $(FLAGS) -o $@ $@.o


clean:
	rm -f *.o *.h.gch $(TARGETS)