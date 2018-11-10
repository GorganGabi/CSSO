#include <iostream>
#include <Windows.h>
#include <ctime>
#include <cstdlib>

using namespace std;


int main()
{
	HANDLE hData;
	unsigned char* pData;
	DWORD a, b;

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

	srand((int)time(0));
	for (int i = 0; i < 200; i++)
	{
		a = (rand() % 1000) + 1;
		b = 2 * a;
		memcpy(pData, &a, sizeof(a));
		pData += sizeof(DWORD);
		memcpy(pData, &b, sizeof(b));
		pData += sizeof(DWORD);
		cout << "Am copiat in fisierul mapat a = " << a << " si b = " << b << endl;
	}

	while (1)
	{
		Sleep(999999);
	}
	CloseHandle(hData);
	CloseHandle(hMutex);
	return 0;
}