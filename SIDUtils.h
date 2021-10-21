#pragma once
#include <Windows.h>

void PrintSIDType(SID_NAME_USE typeCheck);
void PrintSidDetails(PSID psid, const char* wc);
void PrintSidAttributes(DWORD attributes);