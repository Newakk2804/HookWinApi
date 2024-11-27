// Injector.cpp
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

void InjectDLL(DWORD processID, const char* dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (hProcess == nullptr) {
        std::cerr << "Error open process!" << std::endl;
        return;
    }

    // Выделяем память в целевом процессе
    void* pRemoteMemory = VirtualAllocEx(hProcess, nullptr, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    WriteProcessMemory(hProcess, pRemoteMemory, (void*)dllPath, strlen(dllPath) + 1, nullptr);

    // Инъекция DLL
    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibraryA("kernel32.dll"), "LoadLibraryA"), pRemoteMemory, 0, nullptr);

    // Ожидание завершения потока
    WaitForSingleObject(hThread, INFINITE);

    // Освобождение ресурсов
    VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
}

int main() {
    const wchar_t* targetProcessName = L"TargetApp.exe";

    // Находим целевой процесс
    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    DWORD targetProcessID = 0;
    if (hSnap != INVALID_HANDLE_VALUE) {
        while (Process32Next(hSnap, &processEntry)) {
            if (wcscmp(processEntry.szExeFile, targetProcessName) == 0) {
                targetProcessID = processEntry.th32ProcessID;
                break;
            }
        }
        CloseHandle(hSnap);
    }

    if (targetProcessID == 0) {
        std::cerr << "Not Found target process!" << std::endl;
        return 1;
    }

    // Путь к вашей DLL
    const char* dllPath = "D:\\Programming\\Hook\\Injector\\build\\HookDll.dll"; // Убедитесь, что путь правильный
    InjectDLL(targetProcessID, dllPath);

    return 0;
}