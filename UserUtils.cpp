#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")

#include <stdio.h>
#include <windows.h> 
#include <lm.h>
#include <winerror.h>
#include "SystemInfoUtils.h"
#include <assert.h>
#include "Lm.h"
#include <vector>

/*  ACF API related (access, user, group)
 *      
 *              Error codes from BASE+119 to BASE+149
 * 
 * /*
 *      NetICanonicalize and NetIType and NetIMakeLMFileName
 *      NetIListCanon and NetINameCheck
 *              Error codes BASE+250 to BASE+269
 *
 * https://raw.githubusercontent.com/tpn/winsdk-10/master/Include/10.0.16299.0/shared/lmerr.h
 */


#define NERR_UserNotFound (NERR_BASE+121)
#define NERR_GroupExists (NERR_BASE+123)
#define NERR_UserExists (NERR_BASE+124)
#define NERR_ResourceExists (NERR_BASE+125)
#define NERR_NotPrimary (NERR_BASE+126)
#define NERR_InvalidComputer   (NERR_BASE+251)

int AddLocalUser(wchar_t* targetHostname, wchar_t* username, wchar_t* password)
{

    //Works on 2000 Server/Client and higher
    USER_INFO_1 ui;
    DWORD dwLevel = 1;
    DWORD dwError = 0;
    NET_API_STATUS nStatus;

    
    ui.usri1_name = username;
    ui.usri1_password = password;

    //#define USER_PRIV_GUEST     0
    //#define USER_PRIV_USER      1
    //#define USER_PRIV_ADMIN     2
    //The above values are only states, _USER is mandatory, this _priv member is read only according to NetUserAdd
    ui.usri1_priv = USER_PRIV_USER;
    ui.usri1_home_dir = NULL;
    ui.usri1_comment = NULL;
    ui.usri1_flags = UF_SCRIPT; // required
    ui.usri1_script_path = NULL;
    //
    // Call the NetUserAdd function, specifying level 1.
    //
    nStatus = NetUserAdd(targetHostname,dwLevel,(LPBYTE)&ui,&dwError);
    
    auto target = targetHostname;

    if (targetHostname == NULL) {
        target = getCurrentSystemHostname();
    }

    if (nStatus == NERR_Success) {
        fwprintf(stderr, L"User { %ws } has been successfully added on { %ws }\n", username, target);
    }
    else if (nStatus == RPC_S_SERVER_UNAVAILABLE) {
        fwprintf(stderr, L"User { %ws }  was not added on { %ws }! RPC_S_SERVER_UNAVAILABLE, is the hostname reachable? { %d }\n", username,target,nStatus);
        return -1;
    }
    else if (nStatus == ERROR_ACCESS_DENIED){
        fwprintf(stderr, L"User { %ws }  was not added on { %ws }! ERROR_ACCSES_DENIED { %d }\n", username,target,nStatus);
        return -1;
    }
    else if (nStatus == NERR_UserExists) {
        fwprintf(stderr, L"User { %ls }  was not added on { %ls }! NERR_UserExists { %d }\n", username,targetHostname,nStatus);
        return -1;
    }
    else if (nStatus == NERR_InvalidComputer) {
        fwprintf(stderr, L"User { %ws }  was not added on { %ws }! NERR_InvalidComputer { %d }\n", username, targetHostname, nStatus);
        return -1;
    }
    else {
        fwprintf(stderr, L"User { %ws }  was not added on { %ws }! A system error has occurred: { %d }\n", username,target,nStatus);
        return -1;
    }
    return 0;
}

int RemoveLocalUser(wchar_t* targetHostname, wchar_t* username) {
    DWORD dwError = 0;
    NET_API_STATUS nStatus;

    nStatus = NetUserDel(targetHostname, username);

    auto target = targetHostname;

    if (targetHostname == NULL) {
        target = getCurrentSystemHostname();
    }

    if (nStatus == NERR_Success) {
        fwprintf(stderr, L"User { %ws } has been successfully deleted on { %ws }\n", username, target);
    }
    else if (nStatus == RPC_S_SERVER_UNAVAILABLE) {
        fwprintf(stderr, L"User { %ws }  was not deleted on { %ws }! RPC_S_SERVER_UNAVAILABLE, is the hostname reachable? { %d }\n", username, target, nStatus);
        return -1;
    }
    else if (nStatus == ERROR_ACCESS_DENIED) {
        fwprintf(stderr, L"User { %ws }  was not deleted on { %ws }! ERROR_ACCSES_DENIED { %d }\n", username, target, nStatus);
        return -1;
    }
    else if (nStatus == NERR_UserNotFound) {
        fwprintf(stderr, L"User { %ws }  was not deleted on { %ws }! NERR_UserNotFound { %d }\n", username, targetHostname, nStatus);
        return -1;
    }
    else if (nStatus == NERR_NotPrimary) {
        fwprintf(stderr, L"User { %ws }  was not deleted on { %ws }! NERR_NotPrimary { %d }\n", username, targetHostname, nStatus);
        return -1;
    }
    else if (nStatus == NERR_InvalidComputer) {
        fwprintf(stderr, L"User { %ws }  was not deleted on { %ws }! NERR_InvalidComputer { %d }\n", username, targetHostname, nStatus);
        return -1;
    }
    else {
        fwprintf(stderr, L"User { %ws }  was not added on { %ws }! A system error has occurred: { %d }\n", username, target, nStatus);
        return -1;
    }
    return 0;

}

int GetLocalUsers(wchar_t* targetHostname) {
    LPUSER_INFO_0 pBuf = NULL;
    LPUSER_INFO_0 pTmpBuf;
    DWORD dwLevel = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD i;
    DWORD dwTotalCount = 0;
    NET_API_STATUS nStatus;
    LPTSTR pszServerName = NULL;

    // The server is not the default local computer.
    //
    
    pszServerName = (LPTSTR)targetHostname;
    wprintf(L"\nLocal User accounts on %s: \n", pszServerName);
    //
    // Call the NetUserEnum function, specifying level 0; 
    //   enumerate global user account types only.
    //
    do // begin do
    {
        nStatus = NetUserEnum((LPCWSTR)pszServerName,
            dwLevel,
            FILTER_NORMAL_ACCOUNT, // global users
            (LPBYTE*)&pBuf,
            dwPrefMaxLen,
            &dwEntriesRead,
            &dwTotalEntries,
            &dwResumeHandle);
        //
        // If the call succeeds,
        //
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
        {
            if ((pTmpBuf = pBuf) != NULL)
            {
                //
                // Loop through the entries.
                //
                for (i = 0; (i < dwEntriesRead); i++)
                {
                    assert(pTmpBuf != NULL);

                    if (pTmpBuf == NULL)
                    {
                        fprintf(stderr, "An access violation has occurred\n");
                        break;
                    }
                    //
                    //  Print the name of the user account.
                    //
                    wprintf(L"\t-- %s\n", pTmpBuf->usri0_name);

                    pTmpBuf++;
                    dwTotalCount++;
                }
            }
        }
        //
        // Otherwise, print the system error.
        //
        else
            fprintf(stderr, "A system error has occurred: %d\n", nStatus);
        //
        // Free the allocated buffer.
        //
        if (pBuf != NULL)
        {
            NetApiBufferFree(pBuf);
            pBuf = NULL;
        }
    }
    // Continue to call NetUserEnum while 
    //  there are more entries. 
    // 
    while (nStatus == ERROR_MORE_DATA); // end do
    //
    // Check again for allocated memory.
    //
    if (pBuf != NULL)
        NetApiBufferFree(pBuf);
    //
    // Print the final count of users enumerated.
    //
    fprintf(stderr, "\nTotal of %d entries enumerated\n", dwTotalCount);

    return 0;
}

int AddLocalGroup(wchar_t* targetHostname, wchar_t* localgroupName) {

    USER_INFO_1               user_info;
    LOCALGROUP_INFO_1         localgroup_info;
    NET_API_STATUS            err = 0;
    DWORD                     parm_err = 0;


    // Set up the LOCALGROUP_INFO_1 structure. 

    localgroup_info.lgrpi1_name = localgroupName;
    localgroup_info.lgrpi1_comment = (LPWSTR)TEXT("BIGBROTHER");

    err = NetLocalGroupAdd(targetHostname, 1, (LPBYTE)&localgroup_info, &parm_err);

    switch (err)
    {
    case 0:
        printf("Local group successfully created.\n");
        break;
    case ERROR_ALIAS_EXISTS:
        printf("Local group already exists.\n");
        err = 0;
        break;
    case ERROR_INVALID_PARAMETER:
        printf("Invalid parameter error adding local group; parameter index = %d\n",
            err, parm_err);
        return(err);
    default:
        printf("Error adding local group: %d\n", err);
        return(err);
    }
    return(err);
}


int AddUserToLocalGroup(wchar_t* targetHostname, wchar_t* localgroupName, wchar_t* username)
{
        // Now add the user to the local group. 

        NET_API_STATUS err;
        DWORD infoLevel = 1;
        LOCALGROUP_MEMBERS_INFO_3 localgroup_members;
        USER_INFO_1 ui;
        ui.usri1_name = username;
        //NetUserGetInfo(targetHostname, username, infoLevel, (LPBYTE *)&ui);

        localgroup_members.lgrmi3_domainandname = ui.usri1_name;//ui.usri1_name;

        err = NetLocalGroupAddMembers(targetHostname,        // PDC name 
            localgroupName,               // group name 
            3,                            // name 
            (LPBYTE)&localgroup_members, // buffer 
            1);                          // count

        switch (err)
        {
        case 0:
            printf("User successfully added to local group.\n");
            break;
        case ERROR_MEMBER_IN_ALIAS:
            printf("User already in local group.\n");
            err = 0;
            break;
        default:
            printf("Error adding user to local group: %d\n", err);
            break;
        }
        return(err);
}

int RemoveLocalGroup(wchar_t* targetHostname, wchar_t* localgroupName) {
    NET_API_STATUS result;
    result = NetLocalGroupDel(targetHostname, localgroupName);
    switch (result)
    {
    case 0:
        printf("User successfully deleted from local group.\n");
        break;
    default:
        printf("Error adding user to local group: %d\n", result);
        break;
    }
    return(result);
}


int GetLocalGroups(wchar_t* targetHostname)
{
    LOCALGROUP_INFO_0* grpInfo = NULL;
    LOCALGROUP_INFO_0* tempgrpInfo;
    DWORD dwLevel = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    PDWORD_PTR dwResumeHandle = 0;

    wprintf(L"\nLocal Groups %s: \n", targetHostname);

    int res = NetLocalGroupEnum(targetHostname, dwLevel, (LPBYTE*)&grpInfo, dwPrefMaxLen, &dwEntriesRead, &dwTotalEntries, dwResumeHandle);
    if (dwEntriesRead > 0)
    {
        tempgrpInfo = grpInfo;
        for (int i = 0; i < dwEntriesRead; i++)
        {
            wprintf(L"\t-- %s\n", tempgrpInfo->lgrpi0_name);
            tempgrpInfo++;
        }
    }

    if (grpInfo)
    {
        NetApiBufferFree(grpInfo);
        grpInfo = NULL;
    }

    return res;
}


int GetLocalUserLocalGroups(wchar_t * targetHostname, wchar_t* username)
{
    LOCALGROUP_USERS_INFO_0* grpInfo = NULL;
    LOCALGROUP_USERS_INFO_0* tempgrpInfo;
    DWORD dwLevel = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    PDWORD_PTR dwResumeHandle = 0;

    wprintf(L"\nUser %s -- Local Groups %s: \n", username,targetHostname);

    int res = NetUserGetLocalGroups(targetHostname, username, dwLevel, LG_INCLUDE_INDIRECT, (LPBYTE*)&grpInfo, dwPrefMaxLen, &dwEntriesRead, &dwTotalEntries);
    if (dwEntriesRead > 0)
    {
        tempgrpInfo = grpInfo;
        for (int i = 0; i < dwEntriesRead; i++)
        {
            wprintf(L"\t-- %s\n", tempgrpInfo->lgrui0_name);
            tempgrpInfo++;
        }
    }

    if (grpInfo)
    {
        NetApiBufferFree(grpInfo);
        grpInfo = NULL;
    }

    return res;
}