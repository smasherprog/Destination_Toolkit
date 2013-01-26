#include "stdafx.h"
#include "cBaseD3D.h"
#include <sstream>
#include "../Utilities/My_Timer.h"

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

	static cBaseD3D* app =0;
	if( msg == WM_CREATE){
		// Get the 'this' pointer we passed to CreateWindow via the lpParam parameter.
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		app = (cBaseD3D*)cs->lpCreateParams;
		return 0;
	}
	// Don't start processing messages until after WM_CREATE.
	if( app != 0  ) return app->msgProc(msg, wParam, lParam);
	else return DefWindowProc(hwnd, msg, wParam, lParam);
	
}
cBaseD3D::cBaseD3D(HINSTANCE hInstance, std::string appname, unsigned  int height, unsigned int width): MainWnd(0){
	OUTPUT_DEBUG_MSG("Constructing cBaseD3D()");
	mAppPaused=mMinimized=mMaximized=mResizing= false;
	 StartingWidth=width;
	StartingHeight=height;
	AppInst = hInstance;

	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = AppInst;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = "D3DWndClassName";
	wc.hCursor       = LoadCursor(0, IDC_ARROW);

	if( !RegisterClass(&wc) ){
		MessageBox(0, "RegisterClass FAILED", 0, 0);
		PostQuitMessage(0);
	}

	RECT R = { 0, 0, width, height };


	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int other_width  = R.right - R.left;
	int other_height = R.bottom - R.top;

	MainWnd = CreateWindowA("D3DWndClassName", appname.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, other_width, other_height, 0, 0, AppInst, this); 
	if( !MainWnd ){
		MessageBox(0, "CreateWindow FAILED", 0, 0);
		PostQuitMessage(0);
	}
	ShowWindow(MainWnd, SW_SHOW);
	UpdateWindow(MainWnd);
	Graphics::Internal::Init(width, height, MainWnd);
	Current_Height =height;
	Current_Width =width;
	OUTPUT_DEBUG_MSG("Finished Constructing cBaseD3D()");
}
cBaseD3D::~cBaseD3D(){
	OUTPUT_DEBUG_MSG("Calling ~cBaseD3D()");
	Graphics::Internal::DeInit();
	OUTPUT_DEBUG_MSG("Finished ~cBaseD3D()");
}


LRESULT cBaseD3D::msgProc(UINT msg, WPARAM wParam, LPARAM lParam){
	RECT R = { 0, 0, StartingWidth, StartingHeight };
	switch( msg ){
	// WM_ACTIVATE is sent when the window is activated or deactivated.  We pause the game when the window is deactivated and unpause it when it becomes active. 
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )	mAppPaused = true;
		else mAppPaused = false;	
		return 0;
	case WM_SIZE:// WM_SIZE is sent when the user resizes the window.  
		if( Graphics::Internal::Device != 0){
			Current_Height =HIWORD(lParam);
			Current_Width =LOWORD(lParam);
			if( wParam == SIZE_MINIMIZED ){
				mAppPaused = mMinimized = true;
				mMaximized = false;
			}else if( wParam == SIZE_MAXIMIZED ){
				mAppPaused = mMinimized = false;
				mMaximized = true;
				Graphics::Internal::OnResize(Current_Width, Current_Height);// Save the new client area dimensions.
			}else if( wParam == SIZE_RESTORED ){// Restoring from minimized state?
				if( mMinimized ){
					mAppPaused = mMinimized = false;
				} else if( mMaximized ){// Restoring from maximized state?
					mAppPaused = mMaximized = false;
				} 
			}
		}
		return 0;
	case WM_ENTERSIZEMOVE:// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		mAppPaused = true;
		mResizing  = true;
		return 0;
	case WM_EXITSIZEMOVE: // WM_EXITSIZEMOVE is sent when the user releases the resize bars. Here we reset everything based on the new window dimensions.
		mAppPaused = false;
		mResizing  = false;
		Graphics::Internal::OnResize(Current_Width, Current_Height);// Save the new client area dimensions.
		return 0;
	case WM_DESTROY:// WM_DESTROY is sent when the window is being destroyed.
		PostQuitMessage(0);
		return 0;
	case WM_MENUCHAR:// The WM_MENUCHAR message is sent when a menu is active and the user presses  a key that does not correspond to any mnemonic or accelerator key. 
        // Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);
	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = R.right - R.left;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = R.bottom - R.top; 
		return 0;
	}
	return DefWindowProc(MainWnd, msg, wParam, lParam);
}
void cBaseD3D::end(){
	Graphics::Internal::Present();
}
bool cBaseD3D::begin(){
	MSG msg = {0};
	if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE )){
		if ( msg.message == WM_QUIT ) return false;	
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	return true;	
}
