#include <Windows.h>
#include <cstdio>
#include <string>
#include <iostream>

using namespace std;

int flag = 0;


void CreateRegistryRecursively1(string path, string kpath)
{
	HANDLE findFirst;
	WIN32_FIND_DATA fInfo;
	HKEY hkResult;
	LPCBYTE lpData;
	string folder_path;
	string key_path;
	string path_substr;
	DWORD size;

	if (flag == 0)
	{
		for (int i = path.length()-3 ; i > 0; i--)
		{
			if (path[i] == '\\')
			{
				path_substr = path.substr(i+1, (path.length()-i-3));
				kpath.append("\\");
				kpath.append(path_substr);
				break;
			}
		}
		flag = 1;
	}
	findFirst = FindFirstFile(path.c_str(), &fInfo);

	while (FindNextFile(findFirst, &fInfo))
	{
		if (fInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(fInfo.cFileName, "..") != 0 && strcmp(fInfo.cFileName, ".") != 0)
			{
				//Pregatim noul path pentru parcurgerea recursiva
				folder_path = path.substr(0, path.length() - 1);
				folder_path.append(fInfo.cFileName);
				folder_path.append("\\*");

				//pregatim noul path pentru cheie
				string key_path = kpath;
				key_path.append("\\");
				key_path.append(fInfo.cFileName);

				if (RegCreateKeyEx(HKEY_CURRENT_USER, key_path.c_str(), 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkResult, NULL) != ERROR_SUCCESS)
				{
					printf("Nu s-a putut crea cheia.\n");
					printf("Path: %s\n", kpath);
					printf("File: %s\n", fInfo.cFileName);
					printf("Error code: %d\n\n", GetLastError);
					CloseHandle(findFirst);
					return;
				}

				CreateRegistryRecursively1(folder_path, key_path);
				//CloseHandle(hkResult);
			}
		}
		else
		{
			if (RegOpenKeyEx(HKEY_CURRENT_USER, kpath.c_str(), 0, KEY_SET_VALUE, &hkResult) != ERROR_SUCCESS)
			{
				printf("Nu s-a putut deschide cheia.\n");
				printf("Path: %s\n", kpath);
				printf("File: %s\n", fInfo.cFileName);
				printf("Error code: %d\n\n", GetLastError);
				CloseHandle(findFirst);
				return;
			}
			//Aflu lungimea fisierului
			HANDLE fHandle;
			string file_path = path.substr(0, path.length()-1);
			file_path = file_path.append(fInfo.cFileName);
			if ((fHandle = CreateFile(file_path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
			{
				cout << "NU SE DESCHIDE " << GetLastError() << endl;
			}
			size = GetFileSize(fHandle, NULL);

			if (RegSetValueEx(hkResult, fInfo.cFileName, 0, REG_DWORD, (const BYTE*)&size, sizeof(DWORD)) != ERROR_SUCCESS)
			{
				printf("Nu s-a putut seta valoarea.\n");
				printf("Path: %s\n", kpath.c_str());
				printf("File: %s\n", fInfo.cFileName);
				printf("Error code: %d\n\n", GetLastError());
				CloseHandle(findFirst);
				return;
			}
			//RegCloseKey(hkResult);
			//CloseHandle(fHandle);
		}
	}
	//CloseHandle(findFirst);
}

int main(int argc, char* argv[])
{
	CreateRegistryRecursively1("C:\\Users\\Gabi\\Documents\\SpellForce2\\*", "SOFTWARE\\CSSO");

	return 0;
}