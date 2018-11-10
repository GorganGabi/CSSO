#include <iostream>
#include <Windows.h>
#include <ctime>
#include <cstdlib>

using namespace std;

DWORD WINAPI WriteToFileMap(HANDLE hMutex, unsigned char* pData)
{
	DWORD a, b;

	srand((int)time(0));
	for (int i = 0; i < 200; i++)
	{
		WaitForSingleObject(hMutex, INFINITE);

		a = (rand() % 1000) + 1;
		b = 2 * a;
		memcpy(pData, &a, sizeof(a));
		pData += sizeof(DWORD);
		memcpy(pData, &b, sizeof(b));
		pData += sizeof(DWORD);

		cout << i + 1 << ". Threadul cu id-ul " << GetCurrentThreadId() <<" scrie in fisierul mapat a = " << a << " si b =" << b << endl;
		ReleaseMutex(hMutex);
	}
	Sleep(INFINITE);
	return TRUE;
}

int main()
{
	HANDLE aThread[2];
	DWORD ThreadID;
	HANDLE hData, hMutex;
	unsigned char* pData;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	if ((hData = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024 * 1024, "data")) == NULL)
	{
		cout << "Cannot create file mapping. Error code: " << GetLastError();
		return -1;
	}


	if ((pData = (unsigned char*)MapViewOfFile(hData, FILE_MAP_WRITE, 0, 0, 0)) == NULL) {
		cout<< "Cannot get pointer to file mapping. Error code: "<< GetLastError();
		CloseHandle(hData);
		return -1;
	}

	if ((hMutex = CreateMutex(NULL, TRUE, "mutex")) == NULL)
	{
		cout << "Cannot create the mutex. Error code: " << GetLastError();
		CloseHandle(hData);
		return -1;
	}

	if (!CreateProcess("C:\\Users\\Gabi\\Documents\\GitHub\\CSSO\\Tema3\\Tema3_2\\Debug\\Tema3_2.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		cout<< "Cannot create process.Error code: " << GetLastError();
		return 0;
	}

	for (int i = 0; i < 1; i++)
	{
		aThread[i] = CreateThread(
			NULL,       // default security attributes
			0,          // default stack size
			(LPTHREAD_START_ROUTINE)WriteToFileMap(hMutex, pData),
			NULL,       // no thread function arguments
			0,          // default creation flags
			&ThreadID); // receive thread identifier

		if (aThread[i] == NULL)
		{
			printf("CreateThread error: %d\n", GetLastError());
			return 1;
		}
	}

	//WaitForMultipleObjects(1, aThread, TRUE, INFINITE);
	WaitForSingleObject(aThread, INFINITE);

	for (int i = 0; i < 1; i++)
		CloseHandle(aThread[i]);

	CloseHandle(hMutex);
	CloseHandle(hData);
	return 0;
}