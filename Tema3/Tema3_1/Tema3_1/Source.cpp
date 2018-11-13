#include <iostream>
#include <Windows.h>
#include <ctime>
#include <cstdlib>

using namespace std;
int user_input;

void WINAPI WriteToFileMap(HANDLE hMutex, unsigned char* pData)
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

		cout << "[Process 1] " << i + 1 << ". Scriem in fisierul mapat a = " << a << " si b = " << b << endl;

		if (ReleaseMutex(hMutex) == 0)
		{
			cout << "[Process 1] Could not release the mutex. Error code: " << GetLastError() << endl;
		}
	}
}

void WINAPI WriteToFileMap(HANDLE hEventWrite, HANDLE hEventCheck, unsigned char* pData)
{
	DWORD a, b;

	srand((int)time(0));
	for (int i = 0; i < 200; i++)
	{
		WaitForSingleObject(hEventCheck, INFINITE);

		a = (rand() % 1000) + 1;
		b = 2 * a;
		memcpy(pData, &a, sizeof(a));
		pData += sizeof(DWORD);
		memcpy(pData, &b, sizeof(b));
		pData += sizeof(DWORD);

		cout << "[Process 1] " << i + 1 << ". Scriem in fisierul mapat a = " << a << " si b = " << b << endl;

		if (!SetEvent(hEventWrite))
		{
			cout << "[Process 1] Nu s-a putut seta event-ul" << endl;
		}
	}
}

int main()
{
	HANDLE aThread[1];
	DWORD ThreadID;
	HANDLE hData, hMutex = NULL, hEventCheck = NULL, hEventWrite = NULL;
	unsigned char* pData;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	cout << "#####################################################" << endl;
	cout << "Alegeti metoda de sincronizare: " << endl;
	cout << "0. Event" << endl;
	cout << "1. Mutex" << endl;
	cout << "#####################################################" << endl;
	cin >> user_input;
	
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

	if (user_input)
	{
		if ((hMutex = CreateMutex(NULL, FALSE, "mu")) == NULL)
		{
			cout << "[Process 1] Cannot create the mutex. Error code: " << GetLastError();
			CloseHandle(hData);
			return -1;
		}
	}
	else
	{
		if ((hEventWrite = CreateEvent(NULL, FALSE, TRUE, "write_event")) == NULL)
		{
			cout << "[Process 1] Cannot create the event. Error code: " << GetLastError();
			CloseHandle(hData);
			return -1;
		}
		if ((hEventCheck = CreateEvent(NULL, FALSE, TRUE, "check_event")) == NULL)
		{
			cout << "[Process 1] Cannot create the event. Error code: " << GetLastError();
			CloseHandle(hData);
			return -1;
		}
	}

	if (!CreateProcess("C:\\Users\\Gabi\\Documents\\GitHub\\CSSO\\Tema3\\Tema3_2\\Debug\\Tema3_2.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		cout<< "[Process 1] Cannot create process.Error code: " << GetLastError();
		return -1;
	}

	if (user_input)
	{
		WriteToFileMap(hMutex, pData);
	}
	else
	{
		WriteToFileMap(hEventWrite, hEventCheck, pData);
	}

	Sleep(INFINITE);
	CloseHandle(hMutex);
	CloseHandle(hEventCheck);
	CloseHandle(hEventWrite);
	CloseHandle(hData);

	return 0;
}