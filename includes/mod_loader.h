#pragma once

typedef void* HMODULE;
typedef void* HINSTANCE;
typedef bool (*ModDefaultFunction)();

HMODULE LoadLibrary(const char* path);
extern "C" int __stdcall FreeLibrary(HMODULE hModule);

template <typename FuncType>
FuncType GetFunctionT(HMODULE hModule, const char* funcName);