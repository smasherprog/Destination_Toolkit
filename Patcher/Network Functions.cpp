#include "stdafx.h"
#include "WinMain.h"
#include "Network Functions.h"
#include <string>
#include "Winbase.h"
#include <sstream>
#include "Tlhelp32.h"
#include <vector>

//////////////////////////////////////////////
//		GLOBAL VARIABLE DECLARATIONS		//
//////////////////////////////////////////////
HINTERNET hInternet( 0 ), hConnect( 0 );						//Internet and FTP Handles
std::vector<char> relative;										//Relative Directory identifier
size_t relativesize;								//Size of Relateive Directory
bool didUpdateSelf;									//Flag to determine whether to update patcher
bool DXIsUpToDate=false;
bool RunTimeIsUpToDate =false;
size_t ConnectionAttempts=0;
#define NOTFINISHED 1
#define DONE 2

int GetFiles(HINTERNET &hConnect){

	WIN32_FIND_DATAA SdirInfo;
	HINTERNET       hFind;
	DWORD           err;

	size_t CurDirSize(0);												// the number of bytes in array.. not the string length
	char CurDir[MAX_PATH], ServDir[MAX_PATH];							// servDir is the servers directory, curdir is the client
	DWORD pt(MAX_PATH);

	if(FtpGetCurrentDirectoryA(hConnect, CurDir, &pt)){
		CurDirSize=strlen(CurDir)*sizeof(char);							// i do not want the null char at the end
		memcpy(ServDir, CurDir, CurDirSize*sizeof(char));				// make a copy before we modify

		for(DWORD i(9); i < pt; i++)// start at the 8 spot 
			CurDir[i - 9] = CurDir[i];
		CurDir[pt - 9] = NULL;
		ServDir[CurDirSize] = 0;
		for(size_t i(0); i!=CurDirSize; i++){
			if(CurDir[i]==47){											// using ascii numbers because I am cool like that!
				CurDir[i]=92;
			}
		}
		SetCurrentDirectoryA( &relative[0]);
		if(CurDirSize>2){												// if we should change directories.. then do it
			char newdir[MAX_PATH];
			memcpy(newdir, &relative[0], relativesize);
			size_t beg(relativesize/sizeof(char));
			size_t end(beg + (CurDirSize/sizeof(char)));
			for( size_t i(0); beg!=end; beg++){
				newdir[beg]=CurDir[i++];
			}
			newdir[end]=0;												// add the null char
			CreateDirectoryA(newdir, NULL);
			SetCurrentDirectoryA(newdir);
		}																// else skip this.. its the first directory
	} else return NOTFINISHED;
	hFind = FtpFindFirstFileA( hConnect, NULL, &SdirInfo, INTERNET_FLAG_DONT_CACHE, 0 );
	if ( hFind == NULL ){
		err = GetLastError( );
		if( err != ERROR_NO_MORE_FILES ) return NOTFINISHED;
		else return DONE;// done listing the directory get out
	}

	if(SdirInfo.cFileName[0]==46){										// we are working on periods!! ewwww, I need a tampon
		InternetFindNextFileA( hFind, (LPVOID) &SdirInfo );
	}
	for( ; InternetFindNextFileA( hFind, (LPVOID) &SdirInfo ); ){
		if( SdirInfo.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY){
			if(strcmp(SdirInfo.cFileName , "incoming")==0){				// skip the file
				continue;
			}
			//WriteInTextBox("Changing directories on the ftp server...");
			if(FtpSetCurrentDirectoryA(hConnect, SdirInfo.cFileName)){
				//WriteInTextBox("Success.");
				InternetCloseHandle(hFind);
				int k = GetFiles(hConnect);		
				if(k==NOTFINISHED) return NOTFINISHED;// try to restart the download
				// recursive function to search through all directories
				//WriteInTextBox("Change directories on the ftp server...");
 				if(FtpSetCurrentDirectoryA(hConnect, ServDir)){			// resuming the search, change directories back
					//WriteInTextBox("Success.");
					char newdir[MAX_PATH];
					memcpy(newdir, &relative[0], relativesize);
					size_t beg(relativesize/sizeof(char));
					size_t end(beg + (CurDirSize/sizeof(char)));
					for( size_t i(0); beg!=end; beg++){
						newdir[beg]=CurDir[i++];
					}
					newdir[end]=0;										// add the null char
					if(!SetCurrentDirectoryA(newdir)){
						WriteInTextBox("Failed to change local directory.");
					}
					char temp[MAX_PATH];
					size_t q(0);
					do{
						temp[q]=SdirInfo.cFileName[q];
					}while(SdirInfo.cFileName[q++]!=0);
					temp[q]=0;											// add null char
					if(!FindFile(hConnect, hFind, temp, SdirInfo)) return NOTFINISHED;// cannot find the file, something is really wrong. Get out now!
					else continue;
				} else return NOTFINISHED;
			} else return NOTFINISHED;
		} else {														// we are dealing with files, not directories
			if(strcmp(SdirInfo.cFileName , ".ftpquota")==0)	continue;		// skip the file	
			WIN32_FIND_DATAA tfile;
			HANDLE t = FindFirstFileA(SdirInfo.cFileName, &tfile);		// this is the file on the local drive
			SYSTEMTIME ltime, stime;
			FileTimeToSystemTime(&tfile.ftLastWriteTime, &ltime);
			FileTimeToSystemTime(&SdirInfo.ftLastWriteTime, &stime); 
			if(t!=INVALID_HANDLE_VALUE){	
				if(tfile.nFileSizeLow == SdirInfo.nFileSizeLow && ltime.wYear >= stime.wYear){// we might have the same file... do better check	
					if(ltime.wMonth >= stime.wMonth && ltime.wDay >= stime.wDay ) continue;	
					// else the file has a day less than the servers.... download file
				}  
			}
			WriteInTextBox(SdirInfo.cFileName, " is out-dated. Patching...");
			if(strcmp(SdirInfo.cFileName, "Installer.exe") == 0) {
				didUpdateSelf = true;
				FtpGetFileA(hConnect, SdirInfo.cFileName, "InstallerNew.exe", false, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY, 0);
			} else {
				FtpGetFileA(hConnect, SdirInfo.cFileName, SdirInfo.cFileName, false, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY, 0);
			}
		}
	} 
	InternetCloseHandle(hFind);
	return DONE;
}

bool FindFile(HINTERNET &hConnect, HINTERNET &hFind, LPCSTR temp, WIN32_FIND_DATAA &SdirInfo){
	hFind = FtpFindFirstFileA( hConnect, NULL, &SdirInfo, INTERNET_FLAG_DONT_CACHE, 0 );
	if ( hFind == NULL ) return false;// done listing the directory get out
	do{
		if(strcmp(temp, SdirInfo.cFileName)==0) return true;	
	}while(InternetFindNextFileA( hFind, (LPVOID) &SdirInfo ));
	return false;
}

//////////////////////////////////////////////
//		CONNECT TO DESTINATION SERVER		//
//////////////////////////////////////////////
int ConnectToFTP()
{
	Sleep(1000);// add a 1 second delay in here
	hInternet = InternetOpen("Patcher", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (!hInternet){
		InternetCloseHandle(hInternet);
		hInternet=0;
        return 1;
    }

	hConnect = InternetConnect(hInternet, "nolimitsdesigns.com", INTERNET_DEFAULT_FTP_PORT, "anonymous@nolimitsdesigns.com", NULL, INTERNET_SERVICE_FTP, 0, 0);
	if (!hConnect)
	{
		InternetCloseHandle(hConnect);
		hConnect=0;
		InternetCloseHandle(hInternet);
		hInternet=0;
        return 1;
    }
	return 0;
}


//////////////////////////////////////////////////
//		DISCONNECT FROM DESTINATION SERVER		//
//////////////////////////////////////////////////
int DisconnectFromFTP(){
	if(hConnect) {
		if(!InternetCloseHandle(hConnect)) _Error("Error closing hConnect, Please restart");
		hConnect=0;
	}
	if(hInternet) {
		if(!InternetCloseHandle(hInternet)) _Error("Error closing hInternet, Please restart");
		hInternet=0;
	}
	
	return 0;
}


//////////////////////////////////////////////////
//		BEGIN DOWNLOADING/UPDATING PROCESS		//
//////////////////////////////////////////////////
DWORD WINAPI DownloadStuff(LPVOID notUsed)
{
	ConnectionAttempts=0;
	SetCurrentDirectory(gameDirectory);
	didUpdateSelf = false;										//Preload flag to false
	
//Connect to FTP or exit the update thread.	
	WriteInTextBox("Connecting to Destination Server...");		//Alert user that we're connecting to an online server	
	error = ConnectToFTP();										//Connect to server now or return error
//  WHY IS there a connect, disconnect, then connect??????? Because if a user has a fire wall, Windows will ask the user for access, then grant access
// this has the side effect of screwing with the connection and making it not viable on the first connect. So, this double connect will fix that.
	Sleep(400);// add a little pause here to make sure I am not hammering the server with a connect, then disconnect
	DWORD DontTimeOut = 0xFFFFFFFF;
	InternetSetOption(hConnect, INTERNET_OPTION_CONNECT_TIMEOUT, (LPVOID) DontTimeOut, sizeof(DontTimeOut));		//Disable timeout on FTP Connection
	InternetSetOption(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, (LPVOID) DontTimeOut, sizeof(DontTimeOut));		//Disable timeout on internet connection
	//add a spin loop here trying to get files until i am successfull... then go
	if (error)													//If there was an error in connecting to the server...
	{
		DisconnectFromFTP();
		WriteInTextBox("Failed to Connect!!");					//..Alert user to failing to connect
		CloseHandle(hDownloadThread);							//Close the Download thread now that we're done updating
		ZeroMemory(&hDownloadThread, sizeof(hDownloadThread));	//Clear the handle memory for verification in the WinProc
		return (DWORD) 1;										//...and return from thread.  Or...
	}

	WriteInTextBox("Connected.");								//...Tell user we got 'er done! lmao


//Disable Time-Out on both internet handles
	


//Allocate 'relative' and get current directory path
	relative.resize(MAX_PATH);
	relativesize = GetCurrentDirectoryA(MAX_PATH, &relative[0]);

	if(relativesize == 0){
		WriteInTextBox("Could not open current directory!  Update Failed.");		//Let user know we don't have access to the directory
		CloseHandle(hDownloadThread);												//Close the Download thread now that we're done updating
		ZeroMemory(&hDownloadThread, sizeof(hDownloadThread));						//Clear the handle memory for verification in the WinProc
		return true;
	}

	relativesize *= sizeof(char);													// i do not want the null char at the end (via Scott)

//Determine which files to download based off the users O.S.
	bool running64Bit = IsUserRunning64Bit();										//Check if user is running 64-bit
	if (running64Bit) {																//If they are...
		BOOL changeDir = FtpSetCurrentDirectory(hConnect, "//Client64//");			//Set root FTP directory to 64-bit client folder
		if (!changeDir) {															//Error out if failed
			_Error("Failed to go to 64-bit directory");
			return true;
		}
	} else {																		//Else, if user is on a 32-bit OS...
		BOOL changeDir = FtpSetCurrentDirectory(hConnect, "//Client32//");			//Set root FTP directory to the 32-bit client folder
		if (!changeDir) {															//Error out if failed
			_Error("Failed to go to 32-bit directory");
			return true;
		}
	}
		

//Download Files, disconnect from FTP Server then Update DirectX
	int k=NOTFINISHED;
	while(k!=DONE && KeepDownloading){
		k = GetFiles(hConnect);		//Get the latest update files from the FTP Server
		if(!KeepDownloading) return true;
	}

	DisconnectFromFTP();															//Disconnect from the Destination FTP Server
	if(!KeepDownloading) return true;
	BeginDXUpdate();																//Start DirectX Update process (Check update, install if necessary)
	if(!KeepDownloading) return true;
	BeginRunTimeUpdate();
//Check for patcher update, then Finish Up
	if(!KeepDownloading) return true;
	if (didUpdateSelf) {															//If patcher updated itself
		BOOL didSetDirectory = SetCurrentDirectory(gameDirectory);					//Reset local directory to the parent game directory
		if (!didSetDirectory) {														//Error out if failed...\DirectX\ \DirectX\ 
			SendMessage(ghWnd, WM_CLOSE, 0, 0);
			return false;
		} else {	//Else, if we reset the directory
			if(DXIsUpToDate){
				system("start InstallerNew.exe /updateSelfRdy");							//Launch new version of the patcher.
				SendMessage(ghWnd, WM_CLOSE, 0, 0);
				return false;														//Return from function while quitting.
			}else {
				system("start InstallerNew.exe /updateSelfNotRdy");							//Launch new version of the patcher.
				SendMessage(ghWnd, WM_CLOSE, 0, 0);
				return false;
			}
		}
	}																		//If we didn't update the patcher
	//WriteInTextBox("Update finished. You may now safely close the patcher .");	//Output message into the information box
													//Close the Download thread now that we're done updating
	SetCurrentDirectoryA(&relative[0]);
	if(DXIsUpToDate && RunTimeIsUpToDate){
		char tes[] = "Play";
		EnableWindow(hUpdateButton, true);
		SetWindowTextA(hUpdateButton, tes);// change the cancel button to a play button
		char tes1[] = "Close";
		SetWindowTextA(hCancelButton, tes1);// change the cancel button to a play button

		WriteInTextBox("Update Complete, Ready to play!");
	}
	hDownloadThread=0;
	return false;
}


//////////////////////////////////////////////
//		WRITE SINGLE STRING IN INFO BOX		//
//////////////////////////////////////////////
void WriteInTextBox(const char* stringToWrite){
	numoflines++;
	WaitForSingleObject(hDownloadMutex, INFINITE);  // no time-out interval

	if(numoflines>20){
		size_t i(begstring);
		for( ; i< szTitle.size(); i++)
			if(szTitle[i]=='\r') break;
		begstring = ++i;
	}
	szTitle+='\r';
	szTitle+=stringToWrite;	
	ReleaseMutex(hDownloadMutex);
	paintRegion();
}


//////////////////////////////////////////////////////
//		WRITE STRING AND VARIABLE IN TEXT BOX		//
//////////////////////////////////////////////////////
void WriteInTextBox(const char* stringToWrite,const char*stringToAppend){
	numoflines++;
	WaitForSingleObject(hDownloadMutex, INFINITE);  // no time-out interval

	if(numoflines>20){
		size_t i(begstring);
		for( ; i< szTitle.size(); i++)
			if(szTitle[i]=='\r') break;
		begstring = ++i;
	}
	szTitle+='\r';
	szTitle+=stringToWrite;
	szTitle+=" ";
	szTitle+=stringToAppend;	
	ReleaseMutex(hDownloadMutex);
	paintRegion();
}

void BeginRunTimeUpdate(){
	RunTimeIsUpToDate=false;
//Identifier declarations and initializations
	char dxUpdateRevision[3];									//Variable to hold the DirectX update revision numbers
	bool needToUpdate = false, updateAvailable = false;								//Flag to detect whether an update is needed
											//Flag to detect whether an update is available
	HANDLE dxInstallerHandle;										//Handle used to detect whether DirectX Installer is running
	ZeroMemory(&dxUpdateRevision, sizeof(dxUpdateRevision));		//Clear revision identifier of garbage values
	ZeroMemory(&dxInstallerHandle, sizeof(dxInstallerHandle));		//Clear Handle Memory of garbage values
	WriteInTextBox("Checking system for C++ Microsoft 2010 RunTime requirements.");	//Alert user to DirectX inspection
	
//Check Update necessity via installed version comparison
	needToUpdate = CheckInstalledRunTimeRevision();
	if (!needToUpdate){
		RunTimeIsUpToDate=true;
		return;										//Error out if fail or if update is unnecessary
	}
	
//Reset the currently selected directory
	BOOL isDirSet = SetCurrentDirectory(gameDirectory);				//Set Current Directory the Patcher directory
	if (!isDirSet) {												//Error out if fail
		WriteInTextBox("Failed to reset directory while checking DXUpdate");
		return;
	}	
	

//Set current directory to the DirectX Run-Time installation
	isDirSet = SetCurrentDirectory(".\\RunTime\\");				//Set Current Directory to the DirectX redistribution folder
	if (!isDirSet) {												//Error out if fail
		WriteInTextBox("Failed to open \\RunTime\\ directory.");
		return;
	}	
	MessageBoxA(ghWnd, "Your system requires a Visual Studio 2010 runtime update. Please allow administrative access at the prompt", "Update Requiered", MB_OK);
	std::string startedfile;
	if(IsUserRunning64Bit()){
		startedfile= "vcredist_x64.exe";
		system("start vcredist_x64.exe /passive /norestart");	
	}else {
		startedfile= "vcredist_x86.exe";
		system("start vcredist_x86.exe /passive /norestart");			
	}	
	while(!FindProcess(startedfile)){
		Sleep(1000);
	}

	while(FindProcess(startedfile)){
		Sleep(1000);
	}

	Sleep(1000);																				//Wait 1 second
	BringWindowToTop(ghWnd);																	//Try to bring window to the foreground
	SetActiveWindow(ghWnd);																	//Try to make Patcher active window
	// do a check afterwards to see if the install was good
	needToUpdate = CheckInstalledRunTimeRevision();
	if (needToUpdate){
		WriteInTextBox("Microsoft C++ Runtime Install was unsuccessul. You will be unable to play the game");
		return;										//Return Successfully
	}
	RunTimeIsUpToDate=true;
}
//////////////////////////////////////
//		BEGIN UPDATING DIRECT X		//
//////////////////////////////////////
void BeginDXUpdate(){
	DXIsUpToDate=false;
//Identifier declarations and initializations
	char dxUpdateRevision[3];									//Variable to hold the DirectX update revision numbers
	bool needToUpdate = false, updateAvailable = false;								//Flag to detect whether an update is needed
											//Flag to detect whether an update is available
	HANDLE dxInstallerHandle;										//Handle used to detect whether DirectX Installer is running
	ZeroMemory(&dxUpdateRevision, sizeof(dxUpdateRevision));		//Clear revision identifier of garbage values
	ZeroMemory(&dxInstallerHandle, sizeof(dxInstallerHandle));		//Clear Handle Memory of garbage values
	WriteInTextBox("Checking system for DirectX requirements.");	//Alert user to DirectX inspection

//Check Update Availability and revision number
	updateAvailable = GetRedistRevision(dxUpdateRevision);
	if (!updateAvailable){
		EnableWindow(hUpdateButton, true);
		return;										//Error out if fail
	}

//Check Update necessity via installed version comparison
	needToUpdate = CheckInstalledRevision(dxUpdateRevision);
	if (!needToUpdate){
		DXIsUpToDate=true;
		return;										//Error out if fail or if update is unnecessary
	}
	
//Reset the currently selected directory
	BOOL isDirSet = SetCurrentDirectory(gameDirectory);				//Set Current Directory the Patcher directory
	if (!isDirSet) {												//Error out if fail
		WriteInTextBox("Failed to reset directory while checking DXUpdate");
		return;
	}	
	

//Set current directory to the DirectX Run-Time installation
	isDirSet = SetCurrentDirectory(".\\DirectX\\");				//Set Current Directory to the DirectX redistribution folder
	if (!isDirSet) {												//Error out if fail
		WriteInTextBox("Failed to open \\DirectX\\ directory.");
		return;
	}	

	MessageBoxA(ghWnd, "Your system requires a Directx June runtime update. Please allow administrative access at the prompt", "Update Requiered", MB_OK);
//Begin installation process
	system("start DXSETUP.exe /silent");													//Launch the DirectX Installer silently
	
	std::string startedfile = "DXSETUP.exe";

	while(!FindProcess(startedfile)){
		Sleep(1000);
	}
	
	while(FindProcess(startedfile)){
		Sleep(1000);
	}
	Sleep(1000);																				//Wait 1 second
	BringWindowToTop(ghWnd);																	//Try to bring window to the foreground
	SetActiveWindow(ghWnd);																	//Try to make Patcher active window
	// do a check afterwards to see if the install was good
	needToUpdate = CheckInstalledRevision(dxUpdateRevision);
	if (needToUpdate){
		WriteInTextBox("Directx Install was unsuccessul. You will be unable to play the game");
		return;										//Return Successfully
	}
	DXIsUpToDate=true;
}


//////////////////////////////////////////////////
//		GET REDISTRIBUTION REVISION VERSION		//
//////////////////////////////////////////////////
bool GetRedistRevision(char* dxUpdateRevision)
{
//Declare and initialize identifiers
	BOOL isDirSet = false;																//Flag for SetCurrentDirectory
	WIN32_FIND_DATA dxUpdate;															//Structure to hold information over DX Update cabinet
	ZeroMemory(&dxUpdate, sizeof(WIN32_FIND_DATA));										//Clear structure memory

//Set current directory to the run-time folder
	isDirSet = SetCurrentDirectory(".\\DirectX\\");									//Set Current Directory to the DirectX redistribution folder
	if (!isDirSet) {																	//Error out if fail
		WriteInTextBox("Failed to open \\DirectX\\ directory.");
		return false;
	}	
	
//Search for the required cabinet library
	HANDLE updateFileHandle = FindFirstFile("*_d3dx*.cab", &dxUpdate);	//Select the d3dx10 runtime cabinet
	if (updateFileHandle == INVALID_HANDLE_VALUE) {										//Error out if fail
		WriteInTextBox("DirectX Redistribution files missing!");
		return false;
	} 
	std::string ext = dxUpdate.cFileName;
	std::string::size_type pos = ext.find_first_of("_");

	if(pos == std::string::npos) return false;// bad bad
	pos = ext.find_first_of("_", pos+1);// find the next one
	if(pos == std::string::npos) return false;// bad bad
//Extract the required revision number for use in CheckInstalledRevision()
	dxUpdateRevision[0] = ext[pos +1];										//Get the Revision number of the Direct X Run-Time cabinet
	dxUpdateRevision[1] = ext[pos+2];										//Revision number
	dxUpdateRevision[2] = NULL;															//NULL delimiter at the end to ensure correct revision value is applied

	return true;																		//Return SUCCESSFULLY
}

bool CheckInstalledRunTimeRevision(){
//Declare and initialize identifiers
	BOOL isDirSet;																					//Flag for directory setting
	LPWSTR sysDirPath;	
	WIN32_FIND_DATAA dxInstalled;																	//Hold file information for installed and patched DX files
	ZeroMemory(&dxInstalled, sizeof(WIN32_FIND_DATAA));												//Clear structure memory

//Get System files directory
	HRESULT getSysDirPath = SHGetKnownFolderPath(FOLDERID_System, 0, NULL, &sysDirPath);				//Get System32 Path
	if (!(getSysDirPath == S_OK)) {																	//Error out if failed
		_Error("Failed to get System32 Path");
		CoTaskMemFree(sysDirPath);
		return false;
	}

//Set system path as the current directory
	isDirSet = SetCurrentDirectoryW(sysDirPath);														//Set Current Directory to the DirectX redistribution folder
	if (!isDirSet) {																				//Error out if failed
		_Error("Failed to set System path");
		CoTaskMemFree(sysDirPath);
		return false;
	}
	CoTaskMemFree(sysDirPath);
	std::string possibles [3] = { "mfc100.dl", "msvcp100.dl", "msvcr100.dl" };
//Calculate the required file name
//Search for the required file and update or bypass it accordingly
	for(size_t i(0); i< 3; i++){
		HANDLE installedFileHandle = FindFirstFileA(possibles[i].c_str(), &dxInstalled);							//Search for required file
		if((installedFileHandle == INVALID_HANDLE_VALUE) || (GetLastError() == ERROR_FILE_NOT_FOUND)){	//If file is not found
			WriteInTextBox("Visual Studio Run-Time update needed.");													//Inform user we need to update DirectX
			return true;																				//Return and update
		}  																	//Return and bypas	
	}
	WriteInTextBox("Visual Studio Run-Time is up-to-date.");													//Otherwise, let them know we're updated
	return false;		
}


//////////////////////////////////////////////////
//		CHECK INSTALLED REVISION VERSION		//
//////////////////////////////////////////////////
bool CheckInstalledRevision(char* dxUpdateRevision){
//Declare and initialize identifiers
	BOOL isDirSet;																					//Flag for directory setting
	LPWSTR sysDirPath;																				//Variable to hold System32 directory
	WIN32_FIND_DATA dxInstalled;																	//Hold file information for installed and patched DX files
	ZeroMemory(&dxInstalled, sizeof(WIN32_FIND_DATA));												//Clear structure memory

//Get System files directory
	HRESULT getSysDirPath = SHGetKnownFolderPath(FOLDERID_System, 0, NULL, &sysDirPath);				//Get System32 Path
	if (!(getSysDirPath == S_OK)) {																	//Error out if failed
		_Error("Failed to get System32 Path");
		return false;
	}

//Set system path as the current directory
	isDirSet = SetCurrentDirectoryW(sysDirPath);														//Set Current Directory to the DirectX redistribution folder
	if (!isDirSet) {																				//Error out if failed
		_Error("Failed to set System path");
		return false;
	}

//Calculate the required file name
	std::string neededFile = "d3dx10_";															//Creating filename variable
	neededFile+= dxUpdateRevision;															//Append revision number to filename variable
	neededFile+= ".dl";																	//Append file extension

//Search for the required file and update or bypass it accordingly
	HANDLE installedFileHandle = FindFirstFile(neededFile.c_str(), &dxInstalled);							//Search for required file
	if((installedFileHandle == INVALID_HANDLE_VALUE) || (GetLastError() == ERROR_FILE_NOT_FOUND))	//If file is not found
	{
		WriteInTextBox("DirectX update needed.");													//Inform user we need to update DirectX
		return true;																				//Return and update
	} else  {
		WriteInTextBox("DirectX is up-to-date. ");													//Otherwise, let them know we're updated
		return false;																				//Return and bypass
	}
}


//////////////////////////////////////////////////
//		CHECK USER SYSTEM FOR 32- OR 64-BIT		//
//////////////////////////////////////////////////
bool IsUserRunning64Bit() {

	std::vector<char> pointlessString(MAX_PATH);
//Run the required loop
	if (GetSystemWow64Directory(&pointlessString[0], MAX_PATH)) {	//Search for sysWOW64 Directory
		return true;											//If found, return that user is running 64-bit Windows
	} else {
		return false;											//Otherwise, return that user is running 32-bit Windows
	}
}