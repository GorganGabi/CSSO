#pragma comment(lib, "advapi32.lib")

#include <windows.h>
#include <stdio.h>
#include <aclapi.h>
#include <tchar.h>
#include <iostream>
#include <sddl.h>
#include <lmcons.h>

using namespace std;


int main()
{
	HKEY hKey = NULL;
	SECURITY_DESCRIPTOR sd;
	if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
	{
		cout << "InitializeSecurityDescriptor error: " << GetLastError() << endl;
		return -1;
	}
	
	DWORD usernameLength = UNLEN;
	LPSTR username = (LPSTR)malloc(usernameLength);
	GetUserName(username, &usernameLength);

	SID *owner = (SID*)new BYTE[SECURITY_MAX_SID_SIZE];
	DWORD owner_size = SECURITY_MAX_SID_SIZE, size = 1024;
	LPSTR DomainName = new TCHAR[1024];
	SID_NAME_USE peUse;
	
	if (!LookupAccountName(NULL, username, owner, &owner_size, DomainName, &size, &peUse))
	{
		cout << "LookupAccountName error: " << GetLastError() << endl;
		return -1;
	}

	SID *everyone = (SID*)new BYTE[SECURITY_MAX_SID_SIZE];
	DWORD everyone_size = SECURITY_MAX_SID_SIZE;
	if (!CreateWellKnownSid(WELL_KNOWN_SID_TYPE::WinWorldSid, NULL, everyone, &everyone_size))
	{
		cout << "CreateWellKnownSid error: " << GetLastError() << endl;
		return -1;
	}

	if (!SetSecurityDescriptorOwner(&sd, owner, FALSE))
	{
		cout << "SetSecurityDescriptorOwner error: " << GetLastError() << endl;
		return -1;
	}

	EXPLICIT_ACCESS ea[2];
	PACL pACL = NULL;
	ZeroMemory(&ea, 2*sizeof(EXPLICIT_ACCESS));
	ea[0].grfAccessPermissions = KEY_READ;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName = (LPTSTR)everyone;

	ea[1].grfAccessPermissions = KEY_ALL_ACCESS;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance = NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[1].Trustee.ptstrName = (LPSTR)owner;

	if (SetEntriesInAcl(2, ea, NULL, &pACL) != ERROR_SUCCESS)
	{
		cout << "SetEntriesInAcl error: " << GetLastError() << endl;
		return -1;
	}

	if (!SetSecurityDescriptorDacl(&sd, TRUE, pACL, FALSE))
	{
		cout << "SetSecurifyDescrptorDacl error: " << GetLastError() << endl;
		return -1;
	}

	_SECURITY_ATTRIBUTES PSECURITY_ATTRIBUTES;
	PSECURITY_ATTRIBUTES.nLength = sizeof(SECURITY_ATTRIBUTES);
	PSECURITY_ATTRIBUTES.lpSecurityDescriptor = &sd;
	PSECURITY_ATTRIBUTES.bInheritHandle = FALSE;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Test\\Test4", 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, &PSECURITY_ATTRIBUTES, &hKey, NULL) != ERROR_SUCCESS)
	{
		cout << "RegCreateKeyEx error: " << GetLastError() <<  endl;
		return -1;
	}

	delete[SECURITY_MAX_SID_SIZE] owner;
	delete[1024] DomainName;
	return 0;
}