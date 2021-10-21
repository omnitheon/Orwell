#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <atlstr.h>
#include <winnt.h>
#include <ShellAPI.h>

#define	MAXFILEPATHLEN	5000


static void ReRunAsAdministrator(void) {
	SHELLEXECUTEINFO info;
	WCHAR fileName[MAXFILEPATHLEN];
	DWORD pathLen = MAXFILEPATHLEN;


	GetModuleFileName(NULL, fileName, pathLen);

	// Structure details https://msdn.microsoft.com/en-us/library/windows/desktop/bb759784(v=vs.85).aspx
	// https://blogs.msdn.microsoft.com/vistacompatteam/2006/09/25/elevate-through-shellexecute/

	info.cbSize = sizeof(SHELLEXECUTEINFO);
	info.fMask = SEE_MASK_DEFAULT;
	info.hwnd = NULL;
	info.lpVerb = _T("runas");
	info.lpFile = fileName;
	info.lpParameters = NULL;
	info.lpDirectory = NULL;
	info.nShow = SW_SHOWNORMAL;

	ShellExecuteEx(&info);  // Also try the simpler ShellExecute
	ExitProcess(0);

}

CStringA enableSeDebugPrivilege(void) {
	LUID	privilegeLuid;
	if (!LookupPrivilegeValue(NULL, _T("SeDebugPrivilege"), &privilegeLuid)) {
		return "[-] INFO Failed in Lookup privilege value\n";
	}

	TOKEN_PRIVILEGES	tkPrivs;

	tkPrivs.PrivilegeCount = 1;
	tkPrivs.Privileges[0].Luid = privilegeLuid;
	tkPrivs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	HANDLE	currentProcessHandle = GetCurrentProcess();
	HANDLE	processToken;

	if (!OpenProcessToken(currentProcessHandle, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &processToken)) {
		return "[-] INFO Failed in OpenProcessToken\n";
	}

	DWORD structSize;
	GetTokenInformation(processToken, TokenPrivileges, NULL, 0, &structSize);

	DWORD structSize2;   
	PTOKEN_PRIVILEGES processTokenPrivs;
	processTokenPrivs = (PTOKEN_PRIVILEGES)malloc(structSize);

	if (!GetTokenInformation(processToken, TokenPrivileges, processTokenPrivs, structSize, &structSize2)) {
		return "[-] ERROR Failed in GetTokenInformation\n";
	}

	PLUID_AND_ATTRIBUTES runner;
	bool seDebugAvailable = false;

	for (DWORD x = 0; x < processTokenPrivs->PrivilegeCount; x++) {
		runner = &processTokenPrivs->Privileges[x];

		// RtlEqualLuid https://msdn.microsoft.com/en-us/library/windows/hardware/ff561842(v=vs.85).aspx
		// LUID:  https://msdn.microsoft.com/en-us/library/windows/desktop/aa379261(v=vs.85).aspx

		if ((runner->Luid.LowPart == privilegeLuid.LowPart) && (runner->Luid.HighPart == privilegeLuid.HighPart)) {
			return "[+] INFO SeDebugPrivilege available\n";
			seDebugAvailable = true;
			break;
		}
	}

	if (!seDebugAvailable) {
		return "[-] ERROR SeDebugPrivilege unavailable, please RunAs Administrator\n";
	}

	if (!AdjustTokenPrivileges(processToken, false, &tkPrivs, 0, NULL, NULL)) {
		return "[-] ERROR Failed in AdjustTokenPrivileges\n";
	}

	return "[+] INFO SeDebugPrivilege is enabled.\n";
}


