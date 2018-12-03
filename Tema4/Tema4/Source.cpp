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

	if ((hInternetOpen = InternetOpen("CSSO_ftp",
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		0)) == NULL)
	{
		cout << "InternetOpen error. Error: " << GetLastError() << endl;
		return -1;
	}

	if ((hInternetOpen1 = InternetOpen("CSSO_http",
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
			cout << FileData.cFileName << endl;
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
							string resource = line.substr(line.find('//', 8)); // /~george.popoiu/CCSO/a.exe
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

							if (!HttpSendRequest(hHttpOpenRequest,
								NULL,
								0,
								NULL,
								NULL))
							{
								cout << "HttpSendRequest error. Error: " << GetLastError() << endl;
								return -1;
							}

							char buffer[256];
							DWORD bytesRead;
							if (!InternetReadFile(hHttpOpenRequest,
								buffer,
								256,
								&bytesRead))
							{
								cout << "InternetReadFile error. Error: " << GetLastError() << endl;
								return -1;
							}
							string exe = line.substr(line.find_last_of('//') + 1);
							ofstream fout(exe);
							fout << buffer;

							/*ofstream outfile;
							outfile.open(exe);
							outfile << buffer;*/

							while (bytesRead)
							{
								memset(buffer, 0, sizeof(buffer));
								if (!InternetReadFile(hHttpOpenRequest,
									buffer,
									256,
									&bytesRead))
								{
									cout << "InternetReadFile error. Error: " << GetLastError() << endl;
									return -1;
								}
								fout << buffer;
								//outfile << buffer;
							}
							fout.close();
							//outfile.close();

							PROCESS_INFORMATION pi;
							STARTUPINFO si;
							memset(&si, 0, sizeof(si));
							si.cb = sizeof(si);

							if (!CreateProcess(exe.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
								cout << "Cannot create process.Error code: " << GetLastError();
								return -1;
							}
							CloseHandle(hHttpOpenRequest);
							CloseHandle(hInternetConnect1);
						}
					}
					fin.close();
				}
			}
		}
	} while (InternetFindNextFile(hFtpFindFirstFile, &FileData));

	CloseHandle(hInternetOpen1);
	CloseHandle(hFtpFindFirstFile);
	CloseHandle(hInternetConnect);
	CloseHandle(hInternetOpen);
	return 1;
}