#pragma once

#include <Windows.h>
DWORD GetTokenInformationLen(HANDLE processToken, TOKEN_INFORMATION_CLASS infoRequired);

void DumpTokenPrivileges(HANDLE processToken);

void DumpTokenUser(HANDLE processToken);

void DumpTokenGroups(HANDLE processToken);

void DumpTokenOwner(HANDLE processToken);

void DumpTokenPrimaryGroup(HANDLE processToken);

void DumpTokenSource(HANDLE processToken);

void DumpTokenType(HANDLE processToken);

void DumpTokenElevation(HANDLE processToken);