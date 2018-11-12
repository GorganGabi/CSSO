#include <iostream>
#include <Windows.h>
#include <ctime>
#include <cstdlib>

using namespace std;

DWORD WINAPI WriteToFileMap(HANDLE hMutex, unsigned char* pData)
{
	DWORD a, b;

	srand((int)time(0));
	for (int i = 0; i < 10; i++)
	{
		WaitForSingleObject(hMutex, INFINITE);

		a = (rand() % 1000) + 1;
		b = 2 * a;
		memcpy(pData, &a, sizeof(a));
		pData += sizeof(DWORD);
		memcpy(pData, &b, sizeof(b));
		pData += sizeof(DWORD);

		cout << i + 1 << ". Threadul cu id-ul " << GetCurrentThreadId() <<" scrie in fisierul mapat a = " << a << " si b = " << b << endl;
		if (ReleaseMutex(hMutex) == 0)
		{
			cout << "[Process 1] Could not release the mutex. Error code: " << GetLastError() << endl;
		}
	}
	return TRUE;
}

int main()
{
	HANDLE aThread[1];
	DWORD ThreadID;
	HANDLE hData, hMutex;
	unsigned char* pData;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	if ((hData = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024 * 1024, "data")) == NULL)
	{
		cout << "[Process 1] Cannot create file mapping. Error code: " << GetLastError();
		return -1;
	}


	if ((pData = (unsigned char*)MapViewOfFile(hData, FILE_MAP_WRITE, 0, 0, 0)) == NULL) {
		cout<< "[Process 1] Cannot get pointer to file mapping. Error code: "<< GetLastError();
		CloseHandle(hData);
		return -1;
	}

	if ((hMutex = CreateMutex(NULL, TRUE, "mutex")) == NULL)
	{
		cout << "[Process 1] Cannot create the mutex. Error code: " << GetLastError();
		CloseHandle(hData);
		return -1;
	}

	if (!CreateProcess("C:\\Users\\Gabi\\Documents\\GitHub\\CSSO\\Tema3\\Tema3_2\\Debug\\Tema3_2.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		cout<< "[Process 1] Cannot create process.Error code: " << GetLastError();
		return 0;
	}
	else {
		cout << "[Process 1] S-a creat procesul! " << endl;
	}

	WriteToFileMap(hMutex, pData);

	system("pause");
	CloseHandle(hMutex);
	CloseHandle(hData);


	return 0;
}