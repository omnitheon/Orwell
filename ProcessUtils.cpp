#pragma once
#include <Windows.h> //Main lib for most WinAPI
#include <Psapi.h> //EnumProccesses lib
#include <Wtsapi32.h>
#include <iostream>
#include <TlHelp32.h> //ToolHelp API header
#include "HashUtils.h"
#include "TokenUtils.h"

int PrintProcessModules(DWORD processID) {
	// https://docs.microsoft.com/en-us/windows/win32/psapi/enumerating-all-modules-for-a-process
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;

	// Get a handle to the process.
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (NULL == hProcess)
		return 1;
	// Get a list of all the modules in this process.
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			if (GetModuleFileNameEx(hProcess, hMods[i], szModName,// Get the full path to the module's file.
				sizeof(szModName) / sizeof(TCHAR)))
			{
				printf("%ws\n", szModName); // Print the module name and handle value.
			}
		}
	}
	CloseHandle(hProcess); // Release the handle to the process.
	return 0;
}

int dumpRunningProcessBaseline() {
	//create a map that stores strings indexed by strings
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) //-1, system is probably low on memory
		printf("INVALID HANDLE");
	 //Structure that holds information about the processes
					   //This works for 64bit processes as well.
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe); // we must set the size for the next function to succeed
	BOOL res = Process32First(hSnapshot, &pe); //This function could fail since it returns a BOOL.

	WCHAR name[MAX_PATH];
	WCHAR parentName[MAX_PATH];

	do {
		DWORD currentProcessID = pe.th32ProcessID;
		DWORD parentProcessID = pe.th32ParentProcessID;
		printf("\n\n**************************************\n");
		printf("FileName: % ws\n", pe.szExeFile);
		printf("PID: % 7u\n", currentProcessID);

		DWORD parentSize = _countof(parentName);
		HANDLE parentProcessHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, parentProcessID);
		if (parentProcessHandle) {
			if (QueryFullProcessImageName(parentProcessHandle, 0, name, &parentSize)) {
				printf("ParentProcessFilePath: %ws\n", name);
			}
		}

		HANDLE processHandle = OpenProcess(MAXIMUM_ALLOWED, FALSE, currentProcessID);

		if (!processHandle) //Protected System Process
		{
			processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, currentProcessID);
		}

		if (processHandle) {
			DWORD size = _countof(name);
			HANDLE processToken;
			if (QueryFullProcessImageName(processHandle, 0, name, &size)) {
				printf("FilePath: %ws\n", name);
				std::string ss = generateHash(name).str();
				if (sizeof(ss) > 0) {
					printf("MD5: %s\n", ss.c_str());
				}
			}
			if (OpenProcessToken(processHandle, MAXIMUM_ALLOWED, &processToken)) {
				DumpTokenUser(processToken);
				DumpTokenOwner(processToken);
				DumpTokenPrimaryGroup(processToken);
				DumpTokenGroups(processToken);
				DumpTokenPrivileges(processToken);
				DumpTokenSource(processToken);
				DumpTokenType(processToken);
				DumpTokenElevation(processToken);
				CloseHandle(processToken);
			}
			CloseHandle(processHandle);
		}
		PrintProcessModules(currentProcessID);
		printf("**************************************\n\n");
	} while (Process32Next(hSnapshot, &pe)); //iterates to the next process while there is an available one

	CloseHandle(hSnapshot); //this is polite, the kernel will kill all handles for us	
	return 0;
}