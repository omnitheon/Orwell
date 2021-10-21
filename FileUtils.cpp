#pragma once
#include <Windows.h> //Main lib for most WinAPI
#include <iostream>
#include <tchar.h>
#include <strsafe.h>
#include "HashUtils.h"


int ListFilesInDirectory(const wchar_t* searchPath)
{
	//https://docs.microsoft.com/en-us/windows/win32/fileio/listing-the-files-in-a-directory
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;


	// Check that the input path plus 3 is not longer than MAX_PATH.
	// Three characters are for the "\*" plus NULL appended below.

	StringCchLength(searchPath, MAX_PATH, &length_of_arg);

	if (length_of_arg > (MAX_PATH - 3))
	{
		_tprintf(TEXT("\nDirectory path is too long.\n"));
		return (-1);
	}

	//_tprintf(TEXT("\nTarget directory is %s\n\n"), searchPath);

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	TCHAR szDirRoot[MAX_PATH];
	StringCchCopy(szDir, MAX_PATH, searchPath);
	StringCchCopy(szDirRoot, MAX_PATH, searchPath);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	// Find the first file in the directory.

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		return dwError;
	}

	// List all the files in the directory with some info about them.

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((ffd.cFileName[0] == 46 and ffd.cFileName[1] == '\0') or (ffd.cFileName[0] == 46 and ffd.cFileName[1] == 46 and ffd.cFileName[2] == '\0')) {
				continue;
			}
			else {
				//_tprintf(TEXT("  %-260ws   <DIR>\n"), ffd.cFileName);
				wchar_t wcs[MAX_PATH + 500];
				//wcscpy_s(wcs, searchPath);
				wcscpy_s(wcs, szDirRoot);
				wcscat_s(wcs, ffd.cFileName);
				wcscat_s(wcs, L"\\");
				//_tprintf(TEXT("\tRecursively searching the above <DIR>...\n"), ffd.cFileName);
				ListFilesInDirectory(wcs);
			}
		}
		else {
			wchar_t wcs[MAX_PATH + 500];
			wcscpy_s(wcs, szDirRoot);
			wcscat_s(wcs, ffd.cFileName);
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			LONGLONG fsize = filesize.QuadPart;
			if (fsize > 249226752) {
				printf("[TOOLARGE]_%lld....%-100ws\n", fsize, wcs);
				continue;
			}
			else {
				std::string ss = generateHash(wcs).str();
				//printf("%ld_%s_%-100ws\n", fsize, ss.c_str(), wcs);
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		return dwError;
	}

	FindClose(hFind);
	return dwError;
}