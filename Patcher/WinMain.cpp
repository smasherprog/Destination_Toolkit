#include "stdafx.h"
#include "WinMain.h"
#include "Network Functions.h"
#include "resource.h"
#include <sstream>
#include <string>
#include "Tlhelp32.h"

HWND hUpdateButton=0, hCancelButton=0, hwndDlg=0;
HBITMAP	hBKGImage=0;
HANDLE	hDownloadThread(0), hDownloadMutex(0);
DWORD	idDownloadThread;
int error;
HRGN hRgn = NULL;
BITMAP bmpInfo;
HDC hdcDest=0, hdcMem=0;
char gameDirectory[MAX_PATH];
HWND ghWnd = NULL;
std::string szTitle;
size_t begstring(0), numoflines(0);
volatile bool KeepDownloading;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
//////////////////////////////////////////////////////////
//		DEFINE NECESSARY IDENTIFIERS FOR WINMAIN		//
//////////////////////////////////////////////////////////
//	MSG		Msg;												//Message Structure
	WNDCLASSEX	wcPatcher;										//Patcher Window Class


////////////////////////////////////////////////
//		INITIALIZE PATCHER WINDOW CLASS		  //
////////////////////////////////////////////////
	wcPatcher.cbClsExtra	=	NULL;									//NULL extra 
	wcPatcher.cbSize		=	sizeof(WNDCLASSEX);						//Size of this class
	wcPatcher.cbWndExtra	=	NULL;									//NULL extra
	wcPatcher.hbrBackground	=	(HBRUSH) GetStockObject(WHITE_BRUSH);	//Default background
	wcPatcher.hCursor		=	LoadCursor(NULL, IDC_ARROW);			//Default Cursor
	wcPatcher.hIcon			=	LoadIcon(NULL, IDI_WINLOGO);			//Default Icon
	wcPatcher.hIconSm		=	NULL;									//No special small icon
	wcPatcher.hInstance		=	hInstance;								//This isntance
	wcPatcher.lpfnWndProc	=	WinProc;								//Use WinProc() for this window's Message Procedure
	wcPatcher.lpszClassName	=	APP_NAME;								//This name for the Class Name
	wcPatcher.lpszMenuName	=	NULL;									//No Menu Reference
	wcPatcher.style			=	CS_HREDRAW | CS_VREDRAW;				//This Class style

	if(!RegisterClassEx(&wcPatcher))									//Register this WNDCLASSEX or error out if fail
	{
		_Error("Failed to register Patcher Window Class");
		return 1;
	}
	ghWnd = CreateWindow(
		APP_NAME, 
		"Installer", 
		0,
		0, 
		0, 
		100, 
		100, 
		NULL, 
		NULL, 
		hInstance, 
		NULL);

	if (!ghWnd){
		return FALSE;
	}
	hDownloadMutex = CreateMutex( NULL, FALSE,  NULL);  
	hwndDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), ghWnd, (DLGPROC)DialogProc);
	//create region
	createRegion(hwndDlg, hInstance);	

	//////////////////////////////////////////////////////////////////////////////
//					CREATE CLOSE/CANCEL BUTTON OR ERROR OUT					//
//////////////////////////////////////////////////////////////////////////////
	hCancelButton = CreateWindow("Button",											//Create a Button Control class
		"Close",																	//With this text
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,										//These Window Styles
		(APP_WIDTH - 150),															//X location
		750,															//Y location
		100,																		//Button width
		25,																			//Button height
		hwndDlg,																	//Parent Window Handle
		(HMENU) H_CANCEL_BUTTON,													//Handle Reference
		hInstance,																	//Instance Handle
		NULL);																		//LPARAM to this control
	if (!hCancelButton)																//Error out if fail
	{
		_Error("Failed to create Cancel Button window");
		return 1;
	}

////////////////////////////////////////////////////////////////////////////////
//					  CREATE UPDATE BUTTON OR ERROR OUT						  //
////////////////////////////////////////////////////////////////////////////////
	hUpdateButton = CreateWindow("Button",											//Create a Button Control class
		"Update",																	//With this text
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,										//And these properties
		150,															//This X location
		750,															//This Y location
		100,																		//Width
		25,																			//Height
		hwndDlg,																	//Parent Window Handle
		(HMENU) H_UPDATE_BUTTON,													//Menu Handle reference
		hInstance,																	//This instance Handle
		NULL);																		//LPARAM
	if (!hUpdateButton)																//Error out if fail
	{
		_Error("Failed to create Update Button window");
		return 1;
	}




////////////////////////////////////////////////////////////////////////////////
//						DISPLAY PATCHER WINDOW								  //
////////////////////////////////////////////////////////////////////////////////
	ShowWindow(ghWnd, 1);														//Show window
	UpdateWindow(ghWnd);															//WM_PAINT to make sure it displays correctly (Standard procedure)

////////////////////////////////////////////////////////////////////////////////
//				CHECK TO SEE IF WE HAVE PROGRAM ARGUMENTS					  //
////////////////////////////////////////////////////////////////////////////////
	if(strcmp(szCmdLine, "/updateSelfRdy") == 0) {											//Are we Updating the patcher this time?
		SetCurrentDirectory(gameDirectory);												//If so, set directory
		CopyFile("InstallerNew.exe", "Installer.exe", false);							//Copy this new version over the old
		SendMessage(ghWnd, WM_CLOSE, 0, 0);											//Close this version of the patcher
		system("start Installer.exe /updateSelfCompleteRdy");								//Launch the new version of the patcher
		return 0;																		//Return SUCCESSFUL while closing...
	} else if(strcmp(szCmdLine, "/updateSelfNotRdy") == 0) {											//Are we Updating the patcher this time?
		SetCurrentDirectory(gameDirectory);												//If so, set directory
		CopyFile("InstallerNew.exe", "Installer.exe", false);							//Copy this new version over the old
		SendMessage(ghWnd, WM_CLOSE, 0, 0);											//Close this version of the patcher
		system("start Installer.exe /updateSelfCompleteNotRdy");								//Launch the  version of the patcher
		return 0;																		//Return SUCCESSFUL while closing...
	}

	if (strcmp(szCmdLine, "/updateSelfCompleteRdy") == 0) {								//Did we just *finish* updating the patcher?
		DeleteFile("InstallerNew.exe");												//If so, clean up.
		WriteInTextBox("Succesfull self Update ..... Ready to play");		//Print finishing message
		DXIsUpToDate=true;// this is the good to go notice
		char tes[] = "Play";
		SetWindowTextA(hCancelButton, tes);// change the cancel button to a play button
	} else if (strcmp(szCmdLine, "/updateSelfCompleteNotRdy") == 0) {								//Did we just *finish* updating the patcher?
		DeleteFile("InstallerNew.exe");												//If so, clean up.
		WriteInTextBox("Succesfull self Update .....");		//Print finishing message
	}

	MSG msg;
	//message loop

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

	return msg.wParam;
}


void createRegion(HWND hwndDlg, HINSTANCE hInst){
	hdcDest = GetDC(hwndDlg);//get the destination device context
	
	hdcMem = CreateCompatibleDC(NULL);//create a memory DC
	HANDLE hBitmap = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	if(!hBitmap) return;
	GetObject(hBitmap, sizeof(bmpInfo), &bmpInfo);	// Get info about the bitmap 
	
	HGDIOBJ hGdiObj = SelectObject(hdcMem, hBitmap);//Select the bitmap into the d
	
	hRgn = CreateRectRgn(0,0,0,0);//create an empty region
	//Create a region from a bitmap with transparency colour of white
	//change the pixel values for a different transparency color
	//ex - RGB(0,0,0) will mean a transparency color of black.. so the areas
	//of the bitmap not used to create the window will be black
	COLORREF crTransparent = RGB(254, 0, 0);

	int iX = 0;
	int iRet = 0;
	for (int iY = 0; iY < bmpInfo.bmHeight; iY++)
	{
		do
		{
			//skip over transparent pixels at start of lines.
			while (iX < bmpInfo.bmWidth){
				COLORREF te = GetPixel(hdcMem, iX, iY);
				unsigned char* test = reinterpret_cast<unsigned char*>(&te);
				if(test[0] > 247) iX++;
				else break;
			}
			//remember this pixel
			int iLeftX = iX;
			//now find first non transparent pixel
			while (iX < bmpInfo.bmWidth && GetPixel(hdcMem, iX, iY) != crTransparent)
				++iX;
			//create a temp region on this info
			HRGN hRgnTemp = CreateRectRgn(iLeftX, iY, iX, iY+1);
			//combine into main region.
			iRet = CombineRgn(hRgn, hRgn, hRgnTemp, RGN_OR);
			if(iRet == ERROR) return;			
			//delete the temp region for next pass
			DeleteObject(hRgnTemp);
		}while(iX < bmpInfo.bmWidth);
		iX = 0;
	}

	//Centre it on current desktop
	iRet = SetWindowRgn(hwndDlg, hRgn, TRUE);
	if(!iRet) return;
	iX = (GetSystemMetrics(SM_CXSCREEN)) / 2 - (bmpInfo.bmWidth / 2);
	int iY = (GetSystemMetrics(SM_CYSCREEN)) / 2 - (bmpInfo.bmHeight / 2);
	iRet = SetWindowPos(hwndDlg, HWND_TOPMOST, iX, iY, bmpInfo.bmWidth, bmpInfo.bmHeight, NULL);

	SetWindowPos(ghWnd, HWND_BOTTOM, iX + 200, iY+ 200, 10, 10, 0);
	//Copy the memory dc into the screen dc
	paintRegion();
	//delete the bitmap
	DeleteObject(hBitmap);
}

void paintRegion(){
	//transfer color data from the source device context to the destination device context
		

	BitBlt(hdcDest, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, hdcMem, 0, 0, SRCCOPY);
	COLORREF g_rgbText = RGB(0, 0, 50);
	SetTextColor(hdcDest, g_rgbText);
	SetBkMode(hdcDest, TRANSPARENT);
	RECT prc;
	prc.top =340;
	prc.left =310;
	prc.bottom =680;
	prc.right =710;
	WaitForSingleObject(hDownloadMutex, INFINITE);  // no time-out interval
	DrawTextA(hdcDest, szTitle.c_str() + begstring, -1, &prc, DT_NOCLIP);
	ReleaseMutex(hDownloadMutex);	

	RECT r;
	r.top=750;
	r.left = 150;
	r.right = r.left + 100;
	r.bottom = r.top + 25;
	InvalidateRect(hwndDlg, &r, false);
	SendMessage(hUpdateButton, WM_PAINT, 0, true);

	r.top=750;
	r.left = (APP_WIDTH - 150);
	r.right = r.left + 100;
	r.bottom = r.top + 25;

	InvalidateRect(hwndDlg, &r, false);
	SendMessage(hCancelButton, WM_PAINT, 0, true);
}

BOOL mouseState = 0;
POINT CursPt = {0};

INT_PTR CALLBACK DialogProc( HWND hwnd,  UINT uMsg,  WPARAM wParam,LPARAM lParam){
	int ix = 0, iy = 0;
	RECT ret;
	std::ostringstream oks;
	std::string test;
	switch (uMsg){
		case WM_COMMAND:
		 		
				if (LOWORD(wParam) == H_UPDATE_BUTTON){
					char letters[20];

					GetWindowTextA(hUpdateButton, letters,  20);// change the cancel button to a play button
					std::string letter = letters;
					if (letter == "Play") {
						DisconnectFromFTP();
						EndDialog(hwnd,0);	
						
						DestroyWindow(ghWnd);
						bool is64bit = IsUserRunning64Bit();
						if(!is64bit) {
							MessageBoxA(ghWnd, "Destination currently only supports users that use a 64 bit operating system. You are not using a 64 bit operating system, and therefore, cannot run the Destination game. The Patcher will exit after pressing OK. Please check in to http://nolimitsdesigns.com for any future plans to support 32 bit operating systems.", "Error, Non-64 bit OS found", MB_OK | MB_ICONERROR);
						} else {
							if(DXIsUpToDate){
								system("Client.exe");
							}
						}
					}else {		// else, it must say update.. so do it
						EnableWindow(hUpdateButton, false);
						char tes[] = "Cance";
						SetWindowTextA(hCancelButton, tes);// change the cancel button to a play button
						KeepDownloading=true;
						hDownloadThread = CreateThread(NULL, 0, DownloadStuff, 0, NULL, &idDownloadThread);
					}
				}
				if (LOWORD(wParam) == H_CANCEL_BUTTON){	
					KeepDownloading=false;
					if (hDownloadThread) {
						char tes[] = "Close";
						SetWindowTextA(hCancelButton, tes);// change the cancel button to a play button

						CloseHandle(hDownloadThread);
						hDownloadThread=0;
						Sleep(300);
						DisconnectFromFTP();
						WriteInTextBox("Download Canceled ................");
						EnableWindow(hUpdateButton, true); 			
					}else {	// the user must have clicked the close button.. SHUT DOWN	
						DisconnectFromFTP();
						EndDialog(hwnd,0);
						DestroyWindow(ghWnd);
					}
				}
			return 0;

		case WM_PAINT:
			paintRegion();
			return 0;
		case WM_MOVE:
			GetWindowRect(hwnd, &ret);
			SetWindowPos(ghWnd, HWND_TOP, ret.left + 200, ret.top +200, 10, 10, NULL);
			paintRegion();	
			return 0;
		case WM_LBUTTONDOWN:
			GetCursorPos(&CursPt);
			ScreenToClient(hwnd, &CursPt);
			if(CursPt.x >870 && CursPt.x < 890  && CursPt.y > 240  && CursPt.y <270){
				if (hDownloadThread){
					CloseHandle(hDownloadThread);
					hDownloadThread=0;
				}
				Sleep(300);
				DisconnectFromFTP();
				EndDialog(hwnd,0);
				SendMessage(ghWnd,WM_DESTROY,NULL,NULL);
			}
			mouseState = 1;	
			return 0;

		case WM_MOUSEMOVE:
			if(mouseState==1){	
				GetWindowRect(hwnd, &ret);
				SetWindowPos(ghWnd, HWND_TOP, ret.left + 200, ret.top +200, 10, 10, NULL);
				GetCursorPos(&CursPt);
				PostMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM( CursPt.x, CursPt.y));
				mouseState = 0;
				return 0;
			}
	}
	return 0;
}

bool FindProcess(std::string& filename){

	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if( hsnap == INVALID_HANDLE_VALUE ) {
		WriteInTextBox("Error with findprocess");
		return false;
	}

	PROCESSENTRY32 searchcriteria;
	searchcriteria.dwSize = sizeof(PROCESSENTRY32);
	if( !Process32First(hsnap, &searchcriteria) ) {
		CloseHandle(hsnap);
		WriteInTextBox("Error with findprocess");
		return false;
	}
	do{
		std::string f = searchcriteria.szExeFile;	
		if(f == filename) 	return true;
	} while(Process32Next(hsnap, &searchcriteria));
	CloseHandle(hsnap);
	return false;
}