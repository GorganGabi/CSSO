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
	HINTERNET hInternetOpen, hInternetConnect, hHttpOpenRequest, hFtpOpenFile, hFtpFindFirstFile, hInternetFindNextFile;
	char* response[256];
	WIN32_FIND_DATA FileData;

	if ((hInternetOpen = InternetOpen("Filezilla Client",
									  INTERNET_OPEN_TYPE_PRECONFIG,
									  NULL,
									  NULL,
									  0)) == NULL)
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

	if ((hFtpFindFirstFile = FtpFindFirstFile(hInternetConnect,
											 NULL,
											 &FileData,
											 INTERNET_FLAG_RELOAD,
											 NULL)) == NULL)
	{
		cout << "FtpFindFirstFile error. Error: " << GetLastError() << endl;
		return -1;
	}

	do
	{
		if (FileData.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
		{
			//cout << FileData.cFileName << endl;
			if (has_suffix(FileData.cFileName, ".txt"))
			{
				string name = FileData.cFileName;
				string file = "F:\\ftpserver\\" + name;
				ifstream fin(file);
				string line;

				if (fin.is_open())
				{
					while (getline(fin, line))
					{
						if (has_prefix(line, "http") && has_suffix(line, ".exe"))
						{
							cout << file.c_str() << endl;
							/*if (!FtpGetFile(hInternetConnect,
											FileData.cFileName,
											FileData.cFileName,
										    FALSE,
										    FILE_ATTRIBUTE_NORMAL,
										    FTP_TRANSFER_TYPE_ASCII,
								            NULL))
							{
								cout << "Nu am descarcat. Err code: "<< GetLastError() << endl;

								DWORD error;
								char buffer[256];
								DWORD buffer_len = 256;
								InternetGetLastResponseInfo(&error, buffer, &buffer_len);

								cout << "Error buffer: " << endl << buffer << endl;
								return -1;
							}*/
						}
					}
					fin.close();
				}
			}
		}
	} while (InternetFindNextFile(hFtpFindFirstFile, &FileData));

	CloseHandle(hInternetConnect);
	CloseHandle(hInternetOpen);
	return 1;
}