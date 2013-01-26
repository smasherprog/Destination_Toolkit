#include "stdafx.h"

#include "OpenGL.h"
#include "cWidgetBase.h"
#include "Collapsible_List.h"
#include "Button.h"
#include "Scrollable.h"
#include "TextBox.h"
#include "Window.h"
#include "CheckBox.h"
#include "cWidgetSkin.h"
#include <sstream>
#include "Test_Helper.h"
#include "Text.h"


HWND					g_pHWND = NULL;
#pragma comment(lib, "MY_UI_GL_Renderer_Lib")

RECT ClientRect;
MY_UI::Utilities::cInput* input=0;


LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	if(input->ProcessMessage(hwnd, msg, wParam, lParam)) return 0;
	return DefWindowProc(hwnd, msg, wParam, lParam);

}
HWND CreateGameWindow( void )
{
	// very important to allocate this before the window is created because the messageproc above will start to be called immediately after CreateWindowEx is called
	input = new MY_UI::Utilities::cInput();
	WNDCLASS	wc;
	ZeroMemory( &wc, sizeof( wc ) );

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= MainWndProc;
	wc.hInstance		= GetModuleHandle(NULL);
	wc.lpszClassName	= "MY_UI Test";

	RegisterClass( &wc );

	HWND hWindow = CreateWindowEx( (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE) , wc.lpszClassName, "MY_UI -  Open GL Example", (WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN) & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME), 400, 400, 1024, 768, NULL, NULL, GetModuleHandle(NULL), NULL );

	ShowWindow( hWindow, SW_SHOW );
	SetForegroundWindow( hWindow );
	SetFocus( hWindow );

	return hWindow;
}


HGLRC CreateOpenGLDevice()
{
	PIXELFORMATDESCRIPTOR pfd = { 0 };
	pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );    // just its size
	pfd.nVersion = 1;   // always 1

	pfd.dwFlags = PFD_SUPPORT_OPENGL |  // OpenGL support - not DirectDraw
		PFD_DOUBLEBUFFER   |  // double buffering support
		PFD_DRAW_TO_WINDOW;   // draw to the app window, not to a bitmap image

	pfd.iPixelType = PFD_TYPE_RGBA ;    // red, green, blue, alpha for each pixel
	pfd.cColorBits = 24;                // 24 bit == 8 bits for red, 8 for green, 8 for blue.
	// This count of color bits EXCLUDES alpha.

	pfd.cDepthBits = 32;                // 32 bits to measure pixel depth.  
	pfd.cStencilBits=0;
	int pixelFormat = ChoosePixelFormat( GetDC( g_pHWND ), &pfd );

	if ( pixelFormat == 0 )
	{
		FatalAppExit( NULL, TEXT("ChoosePixelFormat() failed!") );
	}    

	SetPixelFormat( GetDC( g_pHWND ), pixelFormat, &pfd );

	HGLRC OpenGLContext = wglCreateContext( GetDC( g_pHWND ) );

	wglMakeCurrent( GetDC( g_pHWND ), OpenGLContext );
	RECT r;
	if ( GetClientRect( g_pHWND, &r ) )
	{
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho( r.left, r.right, r.bottom, r.top, -1.0, 1.0);
		glMatrixMode( GL_MODELVIEW );
		glViewport(0, 0, r.right - r.left, r.bottom - r.top);
	}

	return OpenGLContext;
}

//
// Program starts here
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {	
	g_pHWND = CreateGameWindow();
	HGLRC OpenGLContext = CreateOpenGLDevice();	
	glewInit();

	MY_UI::OpenGL_Renderer* renderer = new MY_UI::OpenGL_Renderer();
	MY_UI::cRootWidget* root = new MY_UI::cRootWidget();
	MY_UI::cWidgetSkin* skin = new MY_UI::cWidgetSkin();
	//the MY_UI::Utilities::cInput input    class declared above will clean up MY UI on its destruction so you dont need to free any memory. You can explicitly call MY_UI::DeInit() if you want when the program is shutting down though
	MY_UI::Init(renderer, skin, root);
	MY_UI::Utilities::SetCursor(MY_UI::Utilities::Standard);
	RECT temp;
	GetClientRect( g_pHWND, &temp );
	root->SetSize(temp.right, temp.bottom);

	InitDemo();
	//InitScene();

	//
	// Begin the main game loop
	//
	MY_UI::Controls::Text* fpscounter = new MY_UI::Controls::Text(root);
	fpscounter->SetFontSize(20);
	unsigned int framecount = 0;
	unsigned int lasttick = MY_UI::Utilities::GetTime();
	std::string fpsStr, FrameStats;

	MSG msg;
	while( true )
	{
		// Skip out if the window is closed
		if ( !IsWindowVisible( g_pHWND ) )
			break;

		// If we have a message from windows..
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			// if it's QUIT then quit..
			if ( msg.message == WM_QUIT )
				break;

			// Handle the regular window stuff..
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			


			framecount++;
			if(MY_UI::Utilities::GetTime() -lasttick  >= 1000){// update
				float fps(static_cast<float>(framecount));
				float msec(1000.0f/fps);
				std::ostringstream outs1;   
				outs1.precision(4);
				outs1<<" FPS: "<<fps<<" MPF: "<<msec;
				fpsStr=outs1.str();
				lasttick=MY_UI::Utilities::GetTime();
				framecount=0;// reset
				std::ostringstream o;
				o<<"Draw Calls: "<<renderer->GetDrawCalls();
				fpscounter->SetText(fpsStr +o.str());
			}

			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			root->Draw();
			SwapBuffers( GetDC( g_pHWND ) );
		}
	}
	MY_UI::Safe_Delete(Dummy);// destroy the dummy. This is not absolutely needed because the MY_UI library will clean up all widgets, but I delete it here because the 3d example needs to be shut down before everything else
	SAFE_DELETE(input);// make sure to delete this here. It will call the cleanup code in its destructor
	// Clean up OpenGL
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( OpenGLContext );

}