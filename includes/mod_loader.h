#pragma once
#include "raylib_windows.h"

typedef int (*ModDefaultFunction)(int);
ModDefaultFunction loadModDll();