#include <iostream>
#include <Windows.h>
#include <ctime>

using namespace std;

void WINAPI WriteToFileMap(HANDLE hMutex, unsigned char* pData)
{
	DWORD a, b;

	srand((int)time(0));
	for (int i = 0; i < 200; i++)
	{
		WaitForSingleObject(hMutex, INFINITE);

		memcpy(&a, pData, sizeof(DWORD));
		pData += sizeof(DWORD);
		memcpy(&b, pData, sizeof(DWORD));
		pData += sizeof(DWORD);
		if (2 * a == b)
			cout <<"[Process 2] " << i + 1 << ". Correct pentru a = " << a << " b = " << b << endl;
		else
			cout <<"[Process 2] " << i + 1 << ". Incorrect pentru a = " << a << " b = " << b << endl;

		ReleaseMutex(hMutex);
	}
}

void WINAPI WriteToFileMap(HANDLE hEventWrite, HANDLE hEventCheck, unsigned char* pData)
{
	DWORD a, b;

	srand((int)time(0));
	for (int i = 0; i < 200; i++)
	{
		WaitForSingleObject(hEventWrite, INFINITE);
	
		a = (rand() % 1000) + 1;
		b = 2 * a;
		memcpy(pData, &a, sizeof(a));
		pData += sizeof(DWORD);
		memcpy(pData, &b, sizeof(b));
		pData += sizeof(DWORD);

		if (2 * a == b)
			cout << "[Process 2] " << i + 1 << ". Correct pentru a = " << a << " b = " << b << endl;
		else
			cout << "[Process 2] " << i + 1 << ". Incorrect pentru a = " << a << " b = " << b << endl;
		SetEvent(hEventCheck);
	}
}

int main()
{
	HANDLE aThread[1];
	DWORD ThreadID;
	HANDLE hData, hMutex = NULL, hEventWrite = NULL, hEventCheck = NULL;
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

	/*if ((hMutex = OpenMutex(SYNCHRONIZE, FALSE, "mu")) == NULL)
	{
		cout << "[Process 2] Cannot open the mutex. Error code: " << GetLastError();
		CloseHandle(hData);
		return -1;
	}*/

	if ((hEventWrite = OpenEvent(SYNCHRONIZE, FALSE, "write_event")) == NULL)
	{
		cout << "[Process 2] Cannot open the event. Error code: " << GetLastError();
		CloseHandle(hData);
		return -1;
	}
	if ((hEventCheck = OpenEvent(EVENT_MODIFY_STATE, FALSE, "check_event")) == NULL)
	{
		cout << "[Process 2] Cannot open the event. Error code: " << GetLastError();
		CloseHandle(hData);
		return -1;
	}
	

	//WriteToFileMap(hMutex, pData);
	WriteToFileMap(hEventWrite, hEventCheck, pData);

	CloseHandle(hMutex);
	CloseHandle(hEventCheck);
	CloseHandle(hEventWrite);
	CloseHandle(hData);

	return 0;
}