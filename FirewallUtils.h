#pragma once
#include <windows.h>
#include <netfw.h>

int driver();
int disableAllFirewallProfiles();

HRESULT WindowsFirewallInitialize(OUT INetFwProfile** fwProfile);

void WindowsFirewallCleanup(IN INetFwProfile* fwProfile);

HRESULT WindowsFirewallIsOn(IN INetFwProfile* fwProfile, OUT BOOL* fwOn);

HRESULT WindowsFirewallTurnOn(IN INetFwProfile* fwProfile);

HRESULT WindowsFirewallTurnOff(IN INetFwProfile* fwProfile);

HRESULT WindowsFirewallAppIsEnabled(IN INetFwProfile* fwProfile, IN const wchar_t* fwProcessImageFileName, OUT BOOL* fwAppEnabled);

HRESULT WindowsFirewallAddApp(IN INetFwProfile* fwProfile, IN const wchar_t* fwProcessImageFileName, IN const wchar_t* fwName);

HRESULT WindowsFirewallPortIsEnabled(IN INetFwProfile* fwProfile, IN LONG portNumber, IN NET_FW_IP_PROTOCOL ipProtocol, OUT BOOL* fwPortEnabled);

HRESULT WindowsFirewallPortAdd(IN INetFwProfile* fwProfile, IN LONG portNumber, IN NET_FW_IP_PROTOCOL ipProtocol, IN const wchar_t* name);;