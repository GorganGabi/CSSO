#include <iostream>
#include <Windows.h>

using namespace std;

int main()
{

	HANDLE hData;
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

	for (int i = 0; i < 200; i++)
	{
		memcpy(&a, pData, sizeof(DWORD));
		pData += sizeof(DWORD);
		memcpy(&b, pData, sizeof(DWORD));
		pData += sizeof(DWORD);
		cout << "Am citit din fisierul mapat a = " << a << " si b = " << b << endl;
	}
	return 1;
}