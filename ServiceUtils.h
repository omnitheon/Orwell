#pragma once
#include <Windows.h>

static BOOL __stdcall StopDependentServices(SC_HANDLE schService, SC_HANDLE schSCManager);
int __stdcall StopServiceProcess(LPCWSTR szSvcName);

int __stdcall StartServiceProcess(LPCWSTR szSvcName);