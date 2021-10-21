#include <windows.h>
#include <stdio.h>
#include <tchar.h>


//OSVERSIONINFO vi = { sizeof(vi) };
//::GetVersionEx(&vi);


UINT getCurrentSystemDirectory() {
    WCHAR systemPath[MAX_PATH];
    return ::GetSystemDirectoryW(systemPath, _countof(systemPath));
}

WCHAR* getCurrentSystemHostname() {

    WCHAR hostname[MAX_COMPUTERNAME_LENGTH];
    DWORD len = _countof(hostname);
    if (::GetComputerName(hostname, &len)) {
        //printf("Hostname: %ws", hostname);
        return hostname;
    }
}