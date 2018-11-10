#include <iostream>
#include <Windows.h>

using namespace std;

int main()
{

	HANDLE hData, hMutex;
	char *pData;
	int a, b;

	if ((hData = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "data")) == NULL)
	{
		printf("OpenFileMapping error. Error code: %d", GetLastError());
		CloseHandle(hData);
		return 0;
	}

	if ((pData = (char*)MapViewOfFile(hData, FILE_MAP_ALL_ACCESS, 0, 0, 0)) == NULL) {
		printf("Cannot get pointer to file mapping. Error code: %d", GetLastError());
		CloseHandle(hData);
		return 0;
	}

	if ((hMutex = OpenMutex(SYNCHRONIZE, TRUE, "mutex")) == NULL)
	{
		cout << "Cannot open the mutex. Error code: " << GetLastError();
		CloseHandle(hData);
		return -1;
	}

	for (int i = 0; i < 200; i++)
	{
		
		WaitForSingleObject(hMutex, INFINITE);

		memcpy(&a, pData, sizeof(DWORD));
		pData += sizeof(DWORD);
		memcpy(&b, pData, sizeof(DWORD));
		pData += sizeof(DWORD);

		cout << i + 1 << ". Threadul cu id-ul " << GetCurrentThreadId() << " citeste din fisierul mapat a = " << a << " si b =" << b << endl;

		ReleaseMutex(hMutex);
	}

	return 0;
}