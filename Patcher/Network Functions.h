#ifndef _NETWORK_FUNCTIONS_H
#define _NETWORK_FUNCTIONS_H

//INCLUDE DIRECTIVES
	#include <Windows.h>
	#include <vector>
	#include <WinInet.h>						//Header needed for internet-based methods
	#include <ShlObj.h>							//Header needed for Shell Object methods
	#pragma comment(lib, "wininet.lib")			//Linked-Library needed for internet-based methods
	#pragma comment(lib, "Shell32.lib")			//Linked-Library needed for Shell Object methods
	
//Method Prototypes
	int ConnectToFTP();											//Method to connect to FTP Server
	int DisconnectFromFTP();									//Method to disconnect from FTP Server
	DWORD WINAPI DownloadStuff(LPVOID);							//Parent function to download game  -- USES IT'S OWN THREAD!
	void StartClient();
	int GetFiles(HINTERNET &hConnect);

	bool FindFile(HINTERNET &hConnect, 
		HINTERNET &hFind, 
		LPCSTR temp, 
		WIN32_FIND_DATAA &SdirInfo);
	void WriteInTextBox(const char* stringToWrite);								//Write string constant into the Text-Box
	void WriteInTextBox(const char*, const char*);						//Write string constant, followed by variable string into TextBox
	void BeginDXUpdate();									//Begin the DirectX Update process
	bool CheckInstalledRunTimeRevision();
	void BeginRunTimeUpdate();
	bool GetRedistRevision(char*);						//Get Redistribution Revision
	bool CheckInstalledRevision(char*);					//Get installed revision
	bool IsUserRunning64Bit();								//Determine OS on user machine

//Global Variables for Network Functions
	extern HINTERNET hInternet, hConnect;			//Internet connection handles
	extern std::vector<char> relative;
	extern size_t relativesize;
	extern bool didUpdateSelf;
#endif
