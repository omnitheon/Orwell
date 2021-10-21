// First get the name of the primary domain controller. 
// Be sure to free the returned buffer. 
/*

err = NetGetDCName(NULL,                    // local computer 
    lpszDomain,                   // domain name 
    (LPBYTE*)&lpszPrimaryDC);  // returned PDC 

if (err != 0)
{
    printf("Error getting DC name: %d\n", err);
    return(err);
}
*/