#include <iostream>
#include <Windows.h>
#include <string>
#include "Source.h"

using namespace std;

struct InfoProces {
	DWORD pid;
	DWORD ppid;
	char  exeName[256];
};

struct ProcessList {
	int         count;
	InfoProces  procese[2048];
}processList;

struct ProcessNode
{
	InfoProces ip;
	DWORD childrenCount = 0;
	ProcessNode* children[200] = { 0 };
	BOOL isRoot;
} *processArb;

BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
	
	if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid))
	{
		cout << "LookupPrivilegeValue error. Err code: " << GetLastError() << endl;
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		cout << "AdjustTokenPrivileges error: " << GetLastError() << endl;
		return FALSE;
	}
	return TRUE;
}

void PrintProcessTree(ProcessNode* root, DWORD level)
{
	DWORD i;
	cout << '\n';
	for (i = 0; i < level; i++)
	{
		cout <<"\t";
	}
	cout << root->ip.exeName << " " << root->ip.pid;

	for (i = 0; i < root->childrenCount; i++)
	{
		PrintProcessTree(root->children[i], level + 1);
	}
}

void KillProcessTree(ProcessNode* root)
{
	DWORD i;
	HANDLE hProcess;

	for (i = 0; i < root->childrenCount; i++)
	{
		KillProcessTree(root->children[i]);
	}

	if ((hProcess = OpenProcess(PROCESS_TERMINATE, false, root->ip.pid)) == INVALID_HANDLE_VALUE)
	{
		cout << "OpenProcess error. Error code: " << GetLastError() << endl;
	}

	if (!TerminateProcess(hProcess, 1))
	{
		cout << "TerminateProcess error. Error code: " << GetLastError() << endl;
	}

	cout << "We have killed " << root->ip.exeName << " " << root->ip.pid;
}

int main()
{
	HANDLE hToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		cout << "OpenProcessToken error. Err code: " << GetLastError() << endl;
	}
	SetPrivilege(hToken, SE_DEBUG_NAME, true);

	HANDLE hData = NULL;
	LPCSTR fileName = "data";
	const char* pData;
	DWORD dwLength;

	if ((hData=OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "data")) == NULL)
	{
		printf("OpenFileMapping error. Error code: %d", GetLastError());
		CloseHandle(hData);
		return 0;
	}

	if ((pData = (const char*)MapViewOfFile(hData, FILE_MAP_ALL_ACCESS, 0, 0, 0)) == NULL) {
		printf("Cannot get pointer to file mapping. Error code: %d", GetLastError());
		CloseHandle(hData);
		return 0;
	}
	
	memcpy(&processList, pData, sizeof(processList));

	processArb = new ProcessNode[processList.count];

	for (int i = 0; i < processList.count; i++)
	{
		processArb[i].ip = processList.procese[i];
		processArb[i].isRoot = true;
		//cout << "[pid]: " << processList.procese[i].pid << " [ppid]: " << processList.procese[i].ppid << " [name]: " << processList.procese[i].exeName << endl;
		for (int j = 0; j < processList.count; j++)
		{
			if (processList.procese[j].pid == processList.procese[i].ppid && i != j)
			{
				//cout << processList.procese[i].exeName << " nu este rad" << endl;
				processArb[i].isRoot = false;
				break;
			}
		}
	}

	for (int i = 0; i < processList.count; i++)
	{
		for (int j = 0; j < processList.count; j++)
		{
			if (processList.procese[i].pid == processList.procese[j].ppid && i != j)
			{
				processArb[i].children[(processArb[i].childrenCount)++] = &processArb[j];
			}
		}
	}


	int rootIndex = 0;
	for (int i = 0; i < processList.count; i++)
	{
		if (processArb[i].isRoot)
		{
			cout << "Root Index: " << rootIndex;
			PrintProcessTree(&processArb[i], 0);
			cout << '\n';
			rootIndex++;
		}
	}

	int rootToBeKilled, rootCount=0;
	cout << "Alegeti numarul procesului pe care vreti sa il omorati: ";
	cin >> rootToBeKilled;

	for (int i = 0; i < processList.count; i++)
	{
		if (processArb[i].isRoot)
		{
			if (rootCount == rootToBeKilled)
			{
				KillProcessTree(&processArb[i]);
			}
			rootCount++;
		}
	}

	delete processArb;
	CloseHandle(hData);

	return 0;
}