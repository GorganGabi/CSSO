#pragma comment(lib, "Wininet")
#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <fstream>
#include <tchar.h>
#include <cstring>
#include <string>

using namespace std;
bool has_suffix(const string &str, const string &suffix)
{
	return str.size() >= suffix.size() &&
		str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool has_prefix(const string &str, const string &prefix)
{
	return (str.find(prefix) == 0);

}

int main()
{
	HINTERNET hInternetOpen, hInternetOpen1, hInternetConnect, hInternetConnect1, hHttpOpenRequest, hFtpFindFirstFile;
	WIN32_FIND_DATA FileData;

	if ((hInternetOpen = InternetOpen("CSSO_ftp", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)) == NULL)
	{
		cout << "InternetOpen error. Error: " << GetLastError() << endl;
		return -1;
	}

	if ((hInternetOpen1 = InternetOpen("CSSO_http", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)) == NULL)
	{
		cout << "InternetOpen error. Error: " << GetLastError() << endl;
		return -1;
	}

	if ((hInternetConnect = InternetConnect(hInternetOpen,
		"127.0.0.1",
		INTERNET_DEFAULT_FTP_PORT,
		"Gabi",
		"parola",
		INTERNET_SERVICE_FTP,
		INTERNET_FLAG_PASSIVE,
		0)) == NULL)
	{
		cout << "InternetOpenUrl error. Error: " << GetLastError() << endl;
		return -1;
	}

	if ((hFtpFindFirstFile = FtpFindFirstFile(hInternetConnect, NULL, &FileData, INTERNET_FLAG_RELOAD, NULL)) == NULL)
	{
		cout << "FtpFindFirstFile error. Error: " << GetLastError() << endl;
		return -1;
	}

	do
	{
		if (FileData.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
		{
			cout << FileData.cFileName << endl;
			if (has_suffix(FileData.cFileName, ".txt"))
			{
				if (!FtpGetFile(hInternetConnect,
								FileData.cFileName,
								FileData.cFileName, 
								FALSE,
								FILE_ATTRIBUTE_NORMAL,
								FTP_TRANSFER_TYPE_ASCII,
								NULL))
				{
					cout << "FtpGetFile error. Error: " << GetLastError() << endl;
					return -1;
				}
				ifstream fin(FileData.cFileName);
				string line;
				if (fin.is_open())
				{
					while (getline(fin, line))
					{
						if (has_prefix(line, "http") && has_suffix(line, ".exe"))
						{
							string resource = line.substr(line.find('//', 8)); // ~george.popoiu/CCSO/a.exe
							string link = line.substr(7, line.length() - (resource.length() + 7)); //students.info.uaic.ro

							if ((hInternetConnect1 = InternetConnect(hInternetOpen1,
								link.c_str(),
								INTERNET_DEFAULT_HTTP_PORT,
								NULL,
								NULL,
								INTERNET_SERVICE_HTTP,
								NULL,
								0)) == NULL)
							{
								cout << "InternetOpenUrl error. Error: " << GetLastError() << endl;
								return -1;
							}

							if ((hHttpOpenRequest = HttpOpenRequest(hInternetConnect1,
								"GET",
								resource.c_str(),
								"1.1",
								NULL,
								NULL,
								INTERNET_FLAG_RELOAD,
								NULL)) == NULL)
							{
								cout << "HttpOpenRequest error. Error: " << GetLastError() << endl;
								return -1;
							}

							if (!HttpSendRequest(hHttpOpenRequest, NULL, 0, NULL, NULL))
							{
								cout << "HttpSendRequest error. Error: " << GetLastError() << endl;
								return -1;
							}

							string exe = line.substr(line.find_last_of('//') + 1);
							HANDLE hLocalFile = CreateFile(exe.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
							if (hLocalFile == NULL)
							{
								printf("[ERROR] CreateFile : %d\n", GetLastError());
							}

							char buffer[4096];
							DWORD bytesRead, bytesWritten;
							if (!InternetReadFile(hHttpOpenRequest, buffer, 4096, &bytesRead))
							{
								cout << "InternetReadFile error. Error: " << GetLastError() << endl;
								return -1;
							}
							if (WriteFile(hLocalFile, buffer, bytesRead, &bytesWritten, NULL) == 0)
							{
								printf("[ERROR] WriteFile : %d\n", GetLastError());
								break;
							}
							//ofstream fout(exe);
							//fout << buffer;

							while (bytesRead)
							{
								memset(buffer, 0, sizeof(buffer));
								if (!InternetReadFile(hHttpOpenRequest, buffer,	4096, &bytesRead))
								{
									cout << "InternetReadFile error. Error: " << GetLastError() << endl;
									return -1;
								}

								if (WriteFile(hLocalFile, buffer, bytesRead, &bytesWritten, NULL) == 0)
								{
									printf("[ERROR] WriteFile : %d\n", GetLastError());
									break;
								}
								//fout << buffer;
							}
							//fout.close();
							CloseHandle(hLocalFile);

							PROCESS_INFORMATION pi;
							STARTUPINFO si;
							memset(&si, 0, sizeof(si));
							si.cb = sizeof(si);

							if (!CreateProcess(exe.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
								cout << "Cannot create process.Error code: " << GetLastError();
								return -1;
							}
							InternetCloseHandle(hHttpOpenRequest);
							InternetCloseHandle(hInternetConnect1);
						}
					}
					fin.close();
				}
			}
		}
	} while (InternetFindNextFile(hFtpFindFirstFile, &FileData));

	InternetCloseHandle(hInternetOpen1);
	InternetCloseHandle(hFtpFindFirstFile);
	InternetCloseHandle(hInternetConnect);
	InternetCloseHandle(hInternetOpen);
	return 1;
}