// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include "MinHook.h"

// Указатель на оригинальную функцию
typedef int (*Calculate_t)(int, int);
Calculate_t originalCalculate = nullptr;

// Новый обработчик
int HookedCalculate(int a, int b) {
    std::cout << "Hooked called Calculate: " << a << ", " << b << std::endl;;
    return originalCalculate(a, b) * 2; // Например, удваиваем результат
}

// Функция для установки хуков
extern "C" __declspec(dllexport) void SetHook() {
    // Инициализация MinHook
    if (MH_Initialize() != MH_OK) {
        std::cout << "Error initialize MinHook." << std::endl;
        return;
    }

    // Получение адреса целевой функции
    HMODULE hModule = GetModuleHandleA("TargetApp.exe");
    if (hModule) {
        void* pTargetFunction = GetProcAddress(hModule, "Calculate");
        if (MH_CreateHook(pTargetFunction, &HookedCalculate, reinterpret_cast<LPVOID*>(&originalCalculate)) == MH_OK) {
            MH_EnableHook(pTargetFunction);
            std::cout << "Hook success!" << std::endl;
            int res = HookedCalculate(7, 7);
            std::cout << res << std::endl;
        }
        else {
            std::cout << "Error download Hook." << std::endl;
        }
    }
    else {
        std::cout << "not get it module TargetApp." << std::endl;
    }
}

// Функция для освобождения ресурсов
extern "C" __declspec(dllexport) void RemoveHook() {
    if (originalCalculate != nullptr) {
        MH_DisableHook(reinterpret_cast<LPVOID>(originalCalculate));
        MH_RemoveHook(reinterpret_cast<LPVOID>(originalCalculate));
    }
    MH_Uninitialize();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        std::cout << "DLL loaded." << std::endl;
        SetHook();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        std::cout << "Dll unloaded" << std::endl;
        RemoveHook();
        break;
    }
    return TRUE;
}
