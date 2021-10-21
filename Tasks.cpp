#include <windows.h>
#include <string>
#include <stdio.h>
#include <tchar.h>
#include <winhttp.h>
#include <stdio.h>

#pragma comment(lib, "Winhttp.lib")



std::string http(LPCWSTR endpoint_ip, unsigned int port, LPCWSTR uri, LPCWSTR method, wchar_t const* data)
{
    if (method == L"post" or method == L"POST") {
        DWORD data_len = wcslen(data);
        LPCWSTR additionalHeaders = L"Content-Type: application/x-www-form-urlencoded\r\n";
        DWORD headersLength = -1;
    }

    std::string response;

    LPSTR pszOutBuffer;

    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    BOOL  bResults = FALSE;

    HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;

    hSession = WinHttpOpen(L"test",WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0);

    if (hSession) {
        hConnect = WinHttpConnect(hSession,endpoint_ip,port,0);
    }

    if (hConnect) {
        hRequest = WinHttpOpenRequest(hConnect,method,uri,NULL,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,0);
    }

    if (hRequest) {
        bResults = WinHttpSendRequest(hRequest,WINHTTP_NO_ADDITIONAL_HEADERS,0,WINHTTP_NO_REQUEST_DATA,0,0,0);
    }

    if (bResults) {
        bResults = WinHttpReceiveResponse(hRequest, NULL);
    }

    if (bResults)
    {
        do
        {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {}

            pszOutBuffer = new char[dwSize + 1];
            if (!pszOutBuffer)
            {
                dwSize = 0;
            }
            else
            {
                ZeroMemory(pszOutBuffer, dwSize + 1);
                if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {}
                else {
                    response = response + std::string(pszOutBuffer);
                    delete[] pszOutBuffer;
                }
            }
        } while (dwSize > 0);
    }

    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

    return response;
}