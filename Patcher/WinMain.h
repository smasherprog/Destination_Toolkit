#ifndef _WINMAIN_H
#define _WINMAIN_H

//////////////////////////
//		DIRECTIVES		//
//////////////////////////
	#include <Windows.h>
	#include <string>

//////////////////////
//		DEFINES		//
//////////////////////
	#define	SCREEN_WIDTH	GetSystemMetrics(SM_CXSCREEN)
	#define SCREEN_HEIGHT	GetSystemMetrics(SM_CYSCREEN)
	#define	APP_NAME		"Destination Installer"
	#define APP_WIDTH		900
	#define APP_HEIGHT		610


//////////////////////////////
//		MENU RESOURCES		//
//////////////////////////////
	#define H_UPDATE_BUTTON	2
	#define H_CANCEL_BUTTON	3
	#define H_BKG_IMAGE		4


//////////////////////
//		MACROS		//
//////////////////////
	#define	_Error(x)	MessageBox(ghWnd, (x), "Error", MB_OK | MB_ICONERROR);


//////////////////////////////////////
//		FUNCTION DECLARATIONS		//
//////////////////////////////////////
	LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);			//Window Procedure


//////////////////////
//		HANDLES		//
//////////////////////
	extern	HWND	hUpdateButton,
					hCancelButton,
					hwndDlg;
	extern	HBITMAP	hBKGImage;
	extern	HFONT	infoFont;


//////////////////////////
//		THREAD INFO		//
//////////////////////////
	extern	HANDLE	hDownloadThread, hDownloadMutex;
	extern	DWORD	idDownloadThread;


//////////////////////////////////
//		GLOBAL VARIABLES		//
//////////////////////////////////
	extern int error;
	extern char gameDirectory[MAX_PATH];


	void createRegion(HWND hwndDlg, HINSTANCE hInstance);
	void paintRegion();
	extern HRGN hRgn;
	extern HWND ghWnd;
	//dest and memory DCs
	extern HDC hdcDest, hdcMem, hdcMask;
	extern std::string szTitle;
	extern size_t begstring, numoflines;
	extern volatile bool KeepDownloading;
	extern bool DXIsUpToDate, RunTimeIsUpToDate;
	extern size_t ConnectionAttempts;
	INT_PTR CALLBACK DialogProc( HWND hwnd,  UINT uMsg,     WPARAM wParam,  LPARAM lParam  );
	bool FindProcess(std::string& filename);
#endif