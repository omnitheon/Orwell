#include <atlstr.h>
#include <windows.h>

void shellexec(TCHAR* fileObject) {
SHELLEXECUTEINFO sei;
memset(&sei, 0, sizeof(sei));
sei.cbSize = sizeof(sei);

sei.lpFile = L"C:\\Windows\\win.ini";
sei.lpVerb = L"open";
sei.nShow = SW_SHOWNORMAL;

::ShellExecuteEx(&sei);
}

CStringA exec(TCHAR* cmd)
{
    CStringA result;
    HANDLE hPipeRead, hPipeWrite;

    SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;


    if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0))
        return result;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));


    si.cb = sizeof(si);

    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.hStdOutput = hPipeWrite;
    si.hStdError = hPipeWrite;
    si.wShowWindow = SW_HIDE;

    BOOL fSuccess = CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    if (!fSuccess)
    {
        printf("%u\n", GetLastError());
        CloseHandle(hPipeWrite);
        CloseHandle(hPipeRead);
        return result;
    }

    bool bProcessEnded = false;
    for (; !bProcessEnded;)
    {
        bProcessEnded = WaitForSingleObject(pi.hProcess, 50) == WAIT_OBJECT_0;

        for (;;)
        {
            char buf[1024];
            DWORD dwRead = 0;
            DWORD dwAvail = 0;

            if (!::PeekNamedPipe(hPipeRead, NULL, 0, NULL, &dwAvail, NULL))
                break;

            if (!dwAvail)
                break;

            if (!::ReadFile(hPipeRead, buf, min(sizeof(buf) - 1, dwAvail), &dwRead, NULL) || !dwRead)
                break;

            buf[dwRead] = 0;
            result += buf;
        }
    }

    CloseHandle(hPipeWrite);
    CloseHandle(hPipeRead);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return result;
}