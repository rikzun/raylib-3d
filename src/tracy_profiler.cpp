#include "tracy_profiler.h"
#include <iostream>

typedef void* HANDLE;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned long DWORD;

struct STARTUPINFO {
    DWORD cb;
    char* lpReserved;
    char* lpDesktop;
    char* lpTitle;
    DWORD dwX;
    DWORD dwY;
    DWORD dwXSize;
    DWORD dwYSize;
    DWORD dwXCountChars;
    DWORD dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    WORD wShowWindow;
    WORD cbReserved2;
    BYTE* lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
};

struct PROCESS_INFORMATION {
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId;
};

extern "C" int CreateProcessA(
    void* lpApplicationName,
    char* lpCommandLine,
    void* lpProcessAttributes,
    void* lpThreadAttributes,
    int bInheritHandles,
    int dwCreationFlags,
    void* lpEnvironment,
    void* lpCurrentDirectory,
    STARTUPINFO* lpStartupInfo,
    PROCESS_INFORMATION* lpProcessInformation
);

extern "C" int TerminateProcess(HANDLE hProcess, DWORD uExitCode);
extern "C" int CloseHandle(HANDLE hObject);

STARTUPINFO si = { sizeof(STARTUPINFO) };
PROCESS_INFORMATION pi;
bool launched = false;

TracyProfiler::TracyProfiler()
{
    std::cout << "CONSTRUCTOR" << std::endl;
    if (launched) return;
    launched = true;

    CreateProcessA(
        nullptr,
        (char*)"../../tracy-profiler.exe -a 127.0.0.1",
        nullptr,
        nullptr,
        0,
        0,
        nullptr,
        nullptr,
        &si,
        &pi
    );
}

TracyProfiler::~TracyProfiler()
{
    std::cout << "DESTRUCTOR" << std::endl;
    if (!launched) return;
    launched = false;

    TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
