#pragma once
#include <windows.h>
#include <string>

std::string http(LPCWSTR endpoint_ip, unsigned int port, LPCWSTR uri, LPCWSTR method, wchar_t const* data);