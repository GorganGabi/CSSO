#include <iostream>
#include <Windows.h>
#include <ctime>

using namespace std;

DWORD WINAPI WriteToFileMap(HANDLE hMutex, unsigned char* pData)
{
	DWORD a, b;

	srand((int)time(0));
	for (int i = 0; i < 10; i++)
	{
		WaitForSingleObject(hMutex, INFINITE);

		memcpy(&a, pData, sizeof(DWORD));
		pData += sizeof(DWORD);
		memcpy(&b, pData, sizeof(DWORD));
		pData += sizeof(DWORD);
		cout << i + 1 << ". Threadul cu id-ul " << GetCurrentThreadId() << " citeste din fisierul mapat a = " << a << " si b =" << b << endl;

		ReleaseMutex(hMutex);
	}
	return TRUE;
}



int main()
{
	HANDLE aThread[1];
	DWORD ThreadID;
	HANDLE hData, hMutex;
	unsigned char *pData;
	int a, b;
	if ((hData = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "data")) == NULL)
	{
		printf("[Process 2] OpenFileMapping error. Error code: %d", GetLastError());
		CloseHandle(hData);
		return 0;
	}

	if ((pData = (unsigned char*)MapViewOfFile(hData, FILE_MAP_ALL_ACCESS, 0, 0, 0)) == NULL) {
		printf("[Process 2] Cannot get pointer to file mapping. Error code: %d", GetLastError());
		CloseHandle(hData);
		return 0;
	}

	if ((hMutex = OpenMutex(SYNCHRONIZE, FALSE, "mutex")) == NULL)
	{
		cout << "[Process 2] Cannot open the mutex. Error code: " << GetLastError();
		CloseHandle(hData);
		return -1;
	}

	WriteToFileMap(hMutex, pData);

	return 0;
}