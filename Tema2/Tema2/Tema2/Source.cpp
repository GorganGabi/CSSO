#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>

using namespace std;

struct InfoProces {
	DWORD pid;
	DWORD ppid;
	char  exeName[256];
}ip;

struct ProcessList {
	int         count;
	InfoProces  procese[2048];
}processList;

int main()
{
	HANDLE hProcessSnap, hProcess;
	LPCSTR fileName = "data";
	PROCESSENTRY32 pe32;
	int len = 0;
	unsigned char* pData;
	HANDLE hData;

	//cer un snapshot de la procese
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		cout << "CreateToolHelp32Snapshot failed. Err = " << GetLastError();
		return -1;
	}

	//initializez dwSize cu dimensiunea structurii
	pe32.dwSize = sizeof(PROCESSENTRY32);

	//obtin informatii despre primul proces
	if (!Process32First(hProcessSnap, &pe32))
	{
		cout << "Process32First failed. Err = " << GetLastError();
		CloseHandle(hProcessSnap);
		return -1;
	}

	//pregatesc fisierul mapa in memorie
	if ((hData = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024 * 1024, "data")) == NULL) {
		cout << "Cannot create file mapping. Error code: " << GetLastError();
		return -1;
	}

	if ((pData = (unsigned char*)MapViewOfFile(hData, FILE_MAP_WRITE, 0, 0, 0)) == NULL) {
		printf("Cannot get pointer to file mapping. Error code: %d", GetLastError());
		CloseHandle(hData);
		return 0;
	}
	
	do
	{
		//populez structura
		ip.pid = pe32.th32ProcessID;
		ip.ppid = pe32.th32ParentProcessID;
		strcpy_s(ip.exeName, pe32.szExeFile);
		processList.procese[processList.count++] = ip;

		cout << pe32.szExeFile << endl;
		
	} while (Process32Next(hProcessSnap, &pe32));

	memcpy(pData, &processList, sizeof(processList));
	CloseHandle(hProcessSnap);

	while (1)
	{
		Sleep(999999);
	}
	return 0;
}