#include "mod_loader.h"

extern "C" {
    HMODULE __stdcall LoadLibraryA(const char* lpLibFileName);
    HINSTANCE __stdcall GetProcAddress(HMODULE hModule, const char* procName);
}

HMODULE LoadLibrary(const char* path)
{
    return LoadLibraryA(path);
}

template <typename FuncType>
FuncType GetFunctionT(HMODULE hModule, const char* funcName) {
    HINSTANCE funcAddress = GetProcAddress(hModule, funcName);
    return reinterpret_cast<FuncType>(funcAddress);
}

template ModDefaultFunction GetFunctionT<ModDefaultFunction>(HMODULE, const char*);