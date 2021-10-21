#pragma once
#include <Windows.h> //Main lib for most WinAPI
#include <iostream>
#include <sddl.h> //SID Functions

void PrintSIDType(SID_NAME_USE typeCheck);
void PrintSidDetails(PSID psid, const char* wc);
void PrintSidAttributes(DWORD attributes);

#define	MAX_ACCOUNTNAME_LEN	5000
#define MAX_DOMAINNAME_LEN	5000

void PrintSIDType(SID_NAME_USE typeCheck) {

	switch (typeCheck)
	{

	case SidTypeUser:
		printf("SidTypeUser: A user SID.");
		break;

	case SidTypeGroup:
		printf("SidTypeGroup: A group SID");
		break;

	case SidTypeDomain:
		printf("SidTypeDomain: A domain SID");
		break;

	case SidTypeAlias:
		printf("SidTypeAlias: An alias SID");
		break;

	case SidTypeWellKnownGroup:
		printf("SidTypeWellKnownGroup: A SID for a well-known group");
		break;

	case SidTypeDeletedAccount:
		printf("SidTypeDeletedAccount: A SID for a deleted account");
		break;

	case SidTypeInvalid:
		printf("SidTypeInvalid: A SID that is not valid");
		break;

	case SidTypeUnknown:
		printf("SidTypeUnknown: A SID of unknown type");
		break;

	case SidTypeComputer:
		printf("SidTypeComputer: A SID for a computer");
		break;

	case SidTypeLabel:
		printf("SidTypeLabel: A mandatory integrity label SID");
		break;

	}
}

void PrintSidDetails(PSID psid, const char* wc) {
	LPTSTR stringSID = NULL;
	// Let's try and print the SID and the associated accounts
	// Well known: https://support.microsoft.com/en-in/help/243330/well-known-security-identifiers-in-windows-operating-systems
	// Convert SID to string 

	if (ConvertSidToStringSidW(psid, &stringSID)) {
		// Let's also try and get the account name with the SID
		// LookupAccountSid https://msdn.microsoft.com/en-us/library/windows/desktop/aa379166(v=vs.85).aspx

		TCHAR accountName[MAX_ACCOUNTNAME_LEN];
		DWORD bufferLen = MAX_ACCOUNTNAME_LEN;
		TCHAR domainName[MAX_DOMAINNAME_LEN];
		DWORD domainNameBufferLen = MAX_DOMAINNAME_LEN;
		SID_NAME_USE sidType;

		if (LookupAccountSid(NULL, psid, accountName, &bufferLen, domainName, &domainNameBufferLen, &sidType))
		{
			printf("%s_%ws\\%ws_%ws\n", wc, domainName, accountName, stringSID);
			PrintSIDType(sidType);
			printf("\n");
		}
	}
	LocalFree(stringSID);
}

void PrintSidAttributes(DWORD attributes) {
	// Group SID Attributes 
	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa379624(v=vs.85).aspx

	return;
	// We can also use a Macro but giving template code so students can do more processing for each
	// if they want 
	if (attributes & SE_GROUP_ENABLED) {
		printf("SE_GROUP_ENABLED ");
	}
	if (attributes & SE_GROUP_ENABLED_BY_DEFAULT) {
		printf("SE_GROUP_ENABLED_BY_DEFAULT ");
	}
	if (attributes & SE_GROUP_INTEGRITY) {
		printf("SE_GROUP_INTEGRITY ");
	}
	if (attributes & SE_GROUP_INTEGRITY_ENABLED) {
		printf("SE_GROUP_INTEGRITY_ENABLED ");
	}
	if (attributes & SE_GROUP_LOGON_ID) {
		printf("SE_GROUP_LOGON_ID ");
	}
	if (attributes & SE_GROUP_MANDATORY) {
		printf("SE_GROUP_MANDATORY ");
	}
	if (attributes & SE_GROUP_OWNER) {
		printf("SE_GROUP_OWNER ");
	}
	if (attributes & SE_GROUP_RESOURCE) {
		printf("SE_GROUP_RESOURCE ");
	}
	if (attributes & SE_GROUP_USE_FOR_DENY_ONLY) {
		printf("SE_GROUP_USE_FOR_DENY_ONLY ");
	}
}

std::wstring GetUsernameFromSid(PSID sid) { //unicode strings are wstrings
	//nullptr is localsystem
	//sid is just a binary-like structure that is similiar to a string.
	if (sid == nullptr) //or NULL, we dont have enough privs to look at the sid if its null
		return L""; //empty unicode string
	WCHAR name[32], domain[32];
	DWORD lname = _countof(name), ldomain = _countof(domain);
	SID_NAME_USE use; //what kind of SID is it
	if (!LookupAccountSid(nullptr, sid, name, &lname, domain, &ldomain, &use))
		return L""; //empty unicode string

	return std::wstring(domain) + L"\\" + name; //return username with domain
}