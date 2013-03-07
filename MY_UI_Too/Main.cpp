#include "stdafx.h"
#include "Directx11.h"
#include <sstream>
#include "Main.h"
#include "Root.h"
#include "Standard_Skin.h"
#include "Font_Factory.h"
#include "Button.h"
#include "Input.h"

HWND					g_pHWND =nullptr;
ID3D11Device			*g_Device=nullptr;
IDXGISwapChain			*SwapChain =nullptr;
ID3D11RenderTargetView	*BackBufferRTV=nullptr;
ID3D11Texture2D			*BackBuffer=nullptr;
ID3D11DeviceContext		*g_DeviceContext=nullptr;

RECT ClientRect;
MY_UI_Too::Utilities::Input* input=nullptr;


LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	if(input->ProcessMessage(hwnd, msg, wParam, lParam)) return 0;
	return DefWindowProc(hwnd, msg, wParam, lParam);

}
HWND CreateGameWindow( void )
{

	WNDCLASS	wc;
	ZeroMemory( &wc, sizeof( wc ) );

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= MainWndProc;
	wc.hInstance		= GetModuleHandle(NULL);
	wc.lpszClassName	= "MY_UI_Too";


	if(!RegisterClass( &wc )){
		std::string msg = "Error Registering window Code: " + std::to_string(GetLastError());
		MessageBoxA(0, msg.c_str(), "Error Registering window", MB_OK);
		return 0;
	}
	
	RECT R = { 0, 0,  1024, 768};


	AdjustWindowRect(&R, (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE), false);
	int other_width  = R.right - R.left;
	int other_height = R.bottom - R.top;

	HWND hWindow = CreateWindowExA( (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE) , wc.lpszClassName, "MY_UI_Too - Direct11 Example", (WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN) & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME), 400, 400, other_width, other_height, NULL, NULL, GetModuleHandle(NULL), NULL );
	if(hWindow==NULL){
		std::string msg = "Error Creating window Code: " + std::to_string(GetLastError());
		MessageBoxA(0, msg.c_str(), "Error Registering window", MB_OK);
		return 0;
	}
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

	
	input = new MY_UI_Too::Utilities::Input();
	// very important to allocate this before the window is created because the messageproc above will start to be called immediately after CreateWindowEx is called

	g_pHWND = CreateGameWindow();

	CreateD3DDevice();	

	RECT temp;
	GetClientRect( g_pHWND, &temp );

	//Initialize the Library Delete will be called on all of these controls when the Input control is destroyed
	MY_UI_Too::Init(
		new MY_UI_Too::DirectX11( g_Device, g_DeviceContext ),
		new MY_UI_Too::Standard_Skin(),
		new MY_UI_Too::Font_Factory(),
		new MY_UI_Too::Controls::Root(),
		temp.right, temp.bottom);

	MY_UI_Too::Controls::Button* button = new MY_UI_Too::Controls::Button(MY_UI_Too::Internal::Root_Widget);

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



			float ClearColor[4] = { .2f, .1f, 1.0f, 0.0f };

			g_DeviceContext->ClearRenderTargetView( BackBufferRTV, ClearColor );
			MY_UI_Too::Internal::Root_Widget->Draw();
			SwapChain->Present(0, 0);

		}
	}
	SAFE_DELETE(input);// make sure to delete this here. It will call the cleanup the library in its destructor

	HR(SwapChain->SetFullscreenState(false, 0));// this is needed in case full screen is on
	RELEASECOM(BackBufferRTV);
	RELEASECOM(SwapChain);
	RELEASECOM(g_DeviceContext);
	RELEASECOM(g_Device);


}