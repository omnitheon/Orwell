#pragma once
#include <windows.h>

int AddLocalGroup(wchar_t* targetHostname, wchar_t* localgroupName);

int AddLocalUser(wchar_t* targetHostname, wchar_t* username, wchar_t* password);

int AddUserToLocalGroup(wchar_t* targetHostname, wchar_t* localgroupName, wchar_t* username);

int RemoveLocalUser(wchar_t* targetHostname, wchar_t* username);

int GetLocalUsers(wchar_t* targetHostname);

int GetLocalGroups(wchar_t* targetHostname);

int RemoveLocalGroup(wchar_t* targetHostname, wchar_t* localgroupName);

int GetLocalGroups(wchar_t* targetHostname);

int GetLocalUserLocalGroups(wchar_t* targetHostname, wchar_t* username);