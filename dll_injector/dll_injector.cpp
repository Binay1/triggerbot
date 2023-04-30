#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

int main(int argc, char** argv)
{
    const char* dll_path = "C:\\Users\\inbox\\OneDrive\\Documents\\TB\\TB\\Debug\\TB.dll";
    HANDLE snapshot = 0;
    PROCESSENTRY32 pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32);
    // Get a snapshot of all currently running processes
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    // Loop through the process entries to find the game
    Process32First(snapshot, &pe);
    std::cout << "Injection started" << std::endl;
    do
    {
        if (wcscmp(pe.szExeFile, L"ac_client.exe") == 0)
        {
            // Get a pipeline to the game's process
            HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, true, pe.th32ProcessID);
            // Allocate memory for our dll in the game's virtual space
            void* injectionAddress = VirtualAllocEx(process, NULL, strlen(dll_path) + 1, MEM_COMMIT, PAGE_READWRITE);
            // Write down our dll's path in the game virtual memory
            WriteProcessMemory(process, injectionAddress, dll_path, strlen(dll_path) + 1, NULL);
            HMODULE kernel32 = GetModuleHandle(L"kernel32.dll");
            // Create another thread for the game process and make it load our dll
            HANDLE thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(kernel32, "LoadLibraryA"), injectionAddress, 0, NULL);
            // Wait for the thread to finish
            WaitForSingleObject(thread, INFINITE);
            DWORD exitCode;
            GetExitCodeThread(thread, &exitCode);
            std::cout << "DLL injected" << std::endl;
            // Clean up the mess we have made
            VirtualFreeEx(process, injectionAddress, 0, MEM_RELEASE);
            CloseHandle(thread);
            CloseHandle(process);
            break;
        }

    } while (Process32Next(snapshot, &pe));
}