#include "stdafx.h"
#include "WinMain.h"
#include "Network Functions.h"
HFONT infoFont;
bool ShouldBeRunning = true;
LRESULT CALLBACK WinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_CREATE:
		//SendMessage(hInfoBox, EM_SETLIMITTEXT, 0, 0);
		infoFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
		GetCurrentDirectory(MAX_PATH, gameDirectory);
		return 0;

	case WM_DESTROY:

		if (hDownloadThread){
			CloseHandle(hDownloadThread);
			hDownloadThread =0;
		}
		if (hConnect){
			InternetCloseHandle(hConnect);
			hConnect=0;
		}
		if (hInternet){
			InternetCloseHandle(hInternet);
			hInternet=0;
		}
		if (hBKGImage){
			DeleteObject(hBKGImage);
			hBKGImage=0;
		}

		DeleteObject(hRgn);
		//Delete the memory DC
		DeleteDC(hdcMem);
		//delete the dest DC
		DeleteDC(hdcDest);
	
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}