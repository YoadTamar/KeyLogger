CC = gcc
FLAGS = -Wall -g
DLL_FLAGS = -shared -o
EXE_FLAGS = -o
TARGET_DLL = Keylogger.dll
TARGET_EXE = injector.exe

.PHONY: all clean

all: $(TARGET_DLL) $(TARGET_EXE)

$(TARGET_DLL): keylogger.c keylogger.h
	$(CC) $(FLAGS) -shared -o $@ $^

$(TARGET_EXE): injector.c injector.h
	$(CC) $(FLAGS) -o $@ $^

clean:
	rm -f *.o *.h.gch $(TARGET_DLL) $(TARGET_EXE)
