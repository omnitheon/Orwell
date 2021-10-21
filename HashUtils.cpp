#pragma once
#include <Windows.h> //Main lib for most WinAPI
#include <iostream>
#include <sstream>
#define BUFSIZE 1024
#define MD5LEN  16


std::stringstream generateHash(LPCWSTR str)
{
	// https://docs.microsoft.com/en-us/windows/win32/seccrypto/example-c-program--creating-an-md-5-hash-from-file-content
	DWORD dwStatus = 0;
	BOOL bResult = FALSE;
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	HANDLE hFile = NULL;
	BYTE rgbFile[BUFSIZE];
	DWORD cbRead = 0;
	BYTE rgbHash[MD5LEN];
	DWORD cbHash = 0;
	CHAR rgbDigits[] = "0123456789abcdef";
	LPCWSTR filename = str;
	std::stringstream MD5;
	// Logic to check usage goes here.

	hFile = CreateFile(filename,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_BACKUP_SEMANTICS,
		NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		dwStatus = GetLastError();
		if (dwStatus == 32) {
			printf("Error opening file %ws...The file is currently in use by another process....	Error Code: %d\n", filename, dwStatus);
		}
		else if (dwStatus == 5) {
			printf("Error opening file %ws...Access is denied....	Error Code: %d\n", filename, dwStatus);
		}
		else if (dwStatus == 3) {
			printf("Error opening file %ws...The system cannot find the path specified..(TOOLONG?)...	Error Code: %d\n", filename, dwStatus);
		}
		else {
			printf("Error opening file %ws...	Error Code: %d\n", filename, dwStatus);
		}
		return MD5;
	}


	// Get handle to the crypto provider
	if (!CryptAcquireContext(&hProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT))
	{
		dwStatus = GetLastError();
		printf("CryptAcquireContext failed: %d\n", dwStatus);
		CloseHandle(hFile);
		return MD5;
	}

	if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
	{
		dwStatus = GetLastError();
		printf("CryptAcquireContext failed: %d\n", dwStatus);
		CloseHandle(hFile);
		CryptReleaseContext(hProv, 0);
		return MD5;
	}

	while (bResult = ReadFile(hFile, rgbFile, BUFSIZE,
		&cbRead, NULL))
	{
		if (0 == cbRead)
		{
			break;
		}

		if (!CryptHashData(hHash, rgbFile, cbRead, 0))
		{
			dwStatus = GetLastError();
			printf("CryptHashData failed: %d\n", dwStatus);
			CryptReleaseContext(hProv, 0);
			CryptDestroyHash(hHash);
			CloseHandle(hFile);
			return MD5;
		}
	}

	if (!bResult)
	{
		dwStatus = GetLastError();
		if (dwStatus == 33) {
			printf("ReadFile failed: The process cannot access the file because another process has locked a portion of the file. ");
		}
		else {
			printf("ReadFile failed: %d\n", dwStatus);
		}
		CryptReleaseContext(hProv, 0);
		CryptDestroyHash(hHash);
		CloseHandle(hFile);
		return MD5;
	}

	cbHash = MD5LEN;


	if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
	{
		//printf("MD5 hash of file %s is: ", filename);
		for (DWORD i = 0; i < cbHash; i++)
		{
			//printf("%c%c", rgbDigits[rgbHash[i] >> 4], rgbDigits[rgbHash[i] & 0xf]);
			MD5 << rgbDigits[rgbHash[i] >> 4];
			MD5 << rgbDigits[rgbHash[i] & 0xf];

		}
	}
	else
	{
		dwStatus = GetLastError();
		printf("CryptGetHashParam failed: %d\n", dwStatus);
		return MD5;
	}

	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	CloseHandle(hFile);

	return MD5;
}
