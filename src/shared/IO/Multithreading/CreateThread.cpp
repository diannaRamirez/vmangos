#include "CreateThread.h"

#if defined(WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <pthread.h>
#endif

std::thread IO::Multithreading::CreateThread(std::string const& name, std::function<void()> entryFunction)
{
    return std::thread([name, entryFunction = std::move(entryFunction)]()
    {
        IO::Multithreading::RenameCurrentThread(name);
        entryFunction();
    });
}

void IO::Multithreading::RenameCurrentThread(std::string const& name)
{
#if defined(WIN32)
    // Windows part taken from https://stackoverflow.com/a/23899379
    // SetThreadDescription is only supported on >= Win10

    const DWORD MS_VC_EXCEPTION=0x406D1388;
#pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO
    {
        DWORD dwType;       // Must be 0x1000.
        LPCSTR szName;      // Pointer to name (in user addr space).
        DWORD dwThreadID;   // Thread ID (-1=caller thread).
        DWORD dwFlags;      // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)

    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = name.c_str();
    info.dwThreadID = GetCurrentThreadId();
    info.dwFlags = 0;

    __try
    {
        RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
    }
#elif defined(__linux__)
    pthread_setname_np(pthread_self(), name.c_str());
#else
    // It's not too serisous if we cant rename a thread
    #warning "IO::Multithreading::_renameThisThread not supported on your platform"
#endif
}