#pragma once

#include <Windows.h> //Main lib for most WinAPI
#include <iostream>
#include "SIDUtils.h"

#define MAX_PRIV_LEN	10000



DWORD GetTokenInformationLen(HANDLE processToken, TOKEN_INFORMATION_CLASS infoRequired) {
	DWORD structSize;
	GetTokenInformation(processToken, infoRequired, NULL, 0, &structSize);
	return structSize;

}

void DumpTokenPrivileges(HANDLE processToken) {
	// Let's first get the structure size
	DWORD structSize = GetTokenInformationLen(processToken, TokenPrivileges);
	// Lets get'em privileges :) 
	DWORD structSize2;   // should come out of the API with same value as structSize2
	PTOKEN_PRIVILEGES processTokenPrivs;
	processTokenPrivs = (PTOKEN_PRIVILEGES)malloc(structSize);

	if (GetTokenInformation(processToken, TokenPrivileges, processTokenPrivs, structSize, &structSize2)) {
		// Now let us iterate through and dump all privileges
		bool seDebugAvailable = false;
		TCHAR privName[MAX_PRIV_LEN];
		DWORD privNameSize = sizeof(privName);
		for (DWORD x = 0; x < processTokenPrivs->PrivilegeCount; x++) {
			privNameSize = sizeof(privName);
			if (!LookupPrivilegeNameW(NULL, &processTokenPrivs->Privileges[x].Luid, privName, &privNameSize)) {
				printf("Required size: %d\n", privNameSize);
			}
			else {
				printf("%ws_", privName);
				if (processTokenPrivs->Privileges[x].Attributes & SE_PRIVILEGE_ENABLED) {

					printf("ENABLED\n");
				}
				else {
					printf("DISABLED\n");
				}

			}
		}

	}
	free(processTokenPrivs);

}

void DumpTokenUser(HANDLE processToken) {
	DWORD infoLen = GetTokenInformationLen(processToken, TokenUser);
	DWORD returnLen;
	PTOKEN_USER tokenUserInfo = (PTOKEN_USER)malloc(infoLen);
	if (GetTokenInformation(processToken, TokenUser, (LPVOID)tokenUserInfo, infoLen, &returnLen)) {
		PrintSidDetails(tokenUserInfo->User.Sid, "TokenUser");
	}
	free(tokenUserInfo);
}

void DumpTokenGroups(HANDLE processToken) {
	DWORD infoLen = GetTokenInformationLen(processToken, TokenGroups);
	DWORD returnLen;
	PTOKEN_GROUPS tokenGroupsInfo = (PTOKEN_GROUPS)malloc(infoLen);

	if (GetTokenInformation(processToken, TokenGroups, (LPVOID)tokenGroupsInfo, infoLen, &returnLen)) {
		for (DWORD x = 0; x < tokenGroupsInfo->GroupCount; x++) {
			PrintSidDetails(tokenGroupsInfo->Groups[x].Sid, "TokenGroup");
			//printf("\n");
			PrintSidAttributes(tokenGroupsInfo->Groups[x].Attributes);
			//printf("\n");
		}
	}
	free(tokenGroupsInfo);
}

void DumpTokenOwner(HANDLE processToken) {
	DWORD infoLen = GetTokenInformationLen(processToken, TokenOwner);
	DWORD returnLen;
	PTOKEN_OWNER tokenOwnerInfo = (PTOKEN_OWNER)malloc(infoLen);

	if (GetTokenInformation(processToken, TokenOwner, (LPVOID)tokenOwnerInfo, infoLen, &returnLen)) {
		PrintSidDetails(tokenOwnerInfo->Owner, "TokenOwner");
	}
	free(tokenOwnerInfo);
}

void DumpTokenPrimaryGroup(HANDLE processToken) {
	DWORD infoLen = GetTokenInformationLen(processToken, TokenPrimaryGroup);
	DWORD returnLen;
	PTOKEN_PRIMARY_GROUP tokenPrimaryGroupInfo = (PTOKEN_PRIMARY_GROUP)malloc(infoLen);

	if (GetTokenInformation(processToken, TokenPrimaryGroup, (LPVOID)tokenPrimaryGroupInfo, infoLen, &returnLen)) {
		PrintSidDetails(tokenPrimaryGroupInfo->PrimaryGroup, "TokenPrimaryGroup");
	}
	free(tokenPrimaryGroupInfo);
}

void DumpTokenSource(HANDLE processToken) {
	DWORD returnLen;
	PTOKEN_SOURCE tokenSourceInfo = (PTOKEN_SOURCE)malloc(sizeof(TOKEN_SOURCE));

	if (GetTokenInformation(processToken, TokenSource, (LPVOID)tokenSourceInfo, sizeof(TOKEN_SOURCE), &returnLen)) {
		printf("TSource_");
		for (DWORD m = 0; m < 8; m++) {
			if (tokenSourceInfo->SourceName[m]) {
				printf("%c", tokenSourceInfo->SourceName[m]);
			}
		}
		printf("\n");
	}
	free(tokenSourceInfo);
}

void DumpTokenType(HANDLE processToken) {
	DWORD returnLen;
	PTOKEN_TYPE tokenTypeInfo = (PTOKEN_TYPE)malloc(sizeof(TOKEN_TYPE));

	if (GetTokenInformation(processToken, TokenType, (LPVOID)tokenTypeInfo, sizeof(TOKEN_TYPE), &returnLen)) {
		printf("TType_");
		if (*tokenTypeInfo == TokenPrimary) {
			printf("TokenPrimary\n");
		}
		else {
			printf("TokenImpersonation");
			// Let's find the type of Impersonation
			PSECURITY_IMPERSONATION_LEVEL tokenImpersonationLevel = (PSECURITY_IMPERSONATION_LEVEL)malloc(sizeof(SECURITY_IMPERSONATION_LEVEL));
			if (GetTokenInformation(processToken, TokenImpersonationLevel, (LPVOID)tokenImpersonationLevel, sizeof(TOKEN_TYPE), &returnLen)) {
				switch (*tokenImpersonationLevel) {
				case SecurityAnonymous:
					printf("_SecurityAnonymous\n");
					break;
				case SecurityIdentification:
					printf("_SecurityIdentification\n");
					break;
				case SecurityImpersonation:
					printf("_SecurityImpersonation\n");
					break;
				case SecurityDelegation:
					printf("_SecurityDelegation\n");
					break;
				}
			}
		}
	}
	free(tokenTypeInfo);
}

void DumpTokenElevation(HANDLE processToken) {
	DWORD returnLen;
	PTOKEN_ELEVATION tokenElevationInfo = (PTOKEN_ELEVATION)malloc(sizeof(TOKEN_ELEVATION));

	if (GetTokenInformation(processToken, TokenElevation, (LPVOID)tokenElevationInfo, sizeof(TOKEN_ELEVATION), &returnLen)) {
		if (tokenElevationInfo->TokenIsElevated) {
			printf("TElevated_TRUE\n");
		}
		else {
			printf("TElevated_FALSE\n");
		}
	}
	free(tokenElevationInfo);
}
