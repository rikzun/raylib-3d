#include "raylib_windows.h"
#include "mod_loader.h"

ModDefaultFunction loadModDll() {
    HMODULE module = LoadLibraryW(L"./mod.dll");
    if (!module) return nullptr;

    return (ModDefaultFunction)GetProcAddress(module, "multiply");
}