#pragma once

#include <Windows.h>
void QueryKey(HKEY hKey, const wchar_t* hive, const wchar_t* skey);


void registry_driver(DWORD hive, const wchar_t key);