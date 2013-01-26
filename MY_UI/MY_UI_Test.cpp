#include "stdafx.h"
#include "Directx11.h"
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
#include "MY_UI_Test.h"

#pragma comment(lib, "MY_UI_DX_Renderer_Lib")

HWND					g_pHWND = NULL;
ID3D11Device			*g_Device = NULL;
IDXGISwapChain			*SwapChain =0;
ID3D11RenderTargetView	*BackBufferRTV=0;
ID3D11Texture2D			*BackBuffer=0;
ID3D11DeviceContext		*g_DeviceContext=0;

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

	HWND hWindow = CreateWindowEx( (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE) , wc.lpszClassName, "MY_UI - Direct10 Example", (WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN) & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME), 400, 400, 1024, 768, NULL, NULL, GetModuleHandle(NULL), NULL );

	ShowWindow( hWindow, SW_SHOW );
	SetForegroundWindow( hWindow );
	SetFocus( hWindow );

	return hWindow;
}


void CreateD3DDevice()
{
	// this has to be the client rect, not the window rect
	GetClientRect( g_pHWND, &ClientRect );


	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferDesc.Width  = ClientRect.right;
	sd.BufferDesc.Height = ClientRect.bottom;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count   = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 1;
	sd.OutputWindow = g_pHWND;
	sd.Windowed     = true;

	D3D_FEATURE_LEVEL found;

	UINT flags =D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined(DEBUG) | defined (_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// D3D_DRIVER_TYPE_UNKNOWN IS NECESARY WHEN PASSING AN ADAPTOR AS AN ARGUMENT IN THE CREATEDEVICE AND SWAPCHAIN!!!
	HR(D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, 0, 0 , D3D11_SDK_VERSION, &sd, &SwapChain, &g_Device, &found, &g_DeviceContext));

	HR(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &BackBuffer));
	HR(g_Device->CreateRenderTargetView(BackBuffer, NULL, &BackBufferRTV));
	RELEASECOM(BackBuffer);
	
	g_DeviceContext->OMSetRenderTargets(1, &BackBufferRTV, 0);
	// Setup the viewport
	D3D11_VIEWPORT viewport;
	// this should be the window rect, not the client rect...
	RECT temp;
	GetClientRect( g_pHWND, &temp );

	viewport.Width  = (FLOAT)temp.right;
	viewport.Height = (FLOAT)temp.bottom;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY =  0;
	g_DeviceContext->RSSetViewports(1, &viewport);

}

//
// Program starts here
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {	

	g_pHWND = CreateGameWindow();
	CreateD3DDevice();	
	

	MY_UI::DirectX11* renderer = new MY_UI::DirectX11( g_Device, g_DeviceContext );
	MY_UI::cRootWidget* root = new MY_UI::cRootWidget();
	MY_UI::cWidgetSkin* skin = new MY_UI::cWidgetSkin();
	//the MY_UI::Utilities::cInput input    class declared above will clean up MY UI on its destruction so you dont need to free any memory. You can explicitly call MY_UI::DeInit() if you want when the program is shutting down though
	MY_UI::Init(renderer, skin, root);
	MY_UI::Utilities::SetCursor(MY_UI::Utilities::Standard);
	RECT temp;
	GetClientRect( g_pHWND, &temp );
	root->SetSize(temp.right, temp.bottom);

	InitDemo();
	InitScene();

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
			// Normal DirectX rendering loop
				

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
				
	
				float ClearColor[4] = { .2f, .1f, 1.0f, 0.0f };
				
				g_DeviceContext->ClearRenderTargetView( BackBufferRTV, ClearColor );
	
				// This is how easy it is to render

				Render3D();
				root->Draw();
				
				SwapChain->Present(0, 0);

		}
	}
	MY_UI::Safe_Delete(Dummy);// destroy the dummy. This is not absolutely needed because the MY_UI library will clean up all widgets, but I delete it here because the 3d example needs to be shut down before everything else
	SAFE_DELETE(input);// make sure to delete this here. It will call the cleanup code in its destructor
	Cleanup();// clean up the 3d stuff
	HR(SwapChain->SetFullscreenState(false, 0));// this is needed in case full screen is on
	
	RELEASECOM(BackBufferRTV);
	RELEASECOM(SwapChain);
	RELEASECOM(g_DeviceContext);
	RELEASECOM(g_Device);


}