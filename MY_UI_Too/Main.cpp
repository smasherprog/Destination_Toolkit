#include "stdafx.h"
#include "Directx11.h"
#include <sstream>
#include "Main.h"
#include "Root.h"
#include "Standard_Skin.h"
#include "Font_Factory.h"
#include "Input.h"


HWND					g_pHWND =nullptr;
ID3D11Device			*g_Device=nullptr;
IDXGISwapChain			*SwapChain =nullptr;
ID3D11RenderTargetView	*BackBufferRTV=nullptr;
ID3D11Texture2D			*BackBuffer=nullptr;
ID3D11DeviceContext		*g_DeviceContext=nullptr;

RECT ClientRect;
MY_UI_Too::Utilities::Input* input=nullptr;
std::vector<MY_UI_Too::Interfaces::IWidget*> widgets;

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
		input,
		temp.right, temp.bottom, 2048);

	Init();
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



			float ClearColor[4] = {207.0f/255.0f,207.0f/255.0f,231.0f/255.0f, 0.0f };

			g_DeviceContext->ClearRenderTargetView( BackBufferRTV, ClearColor );
			MY_UI_Too::Internal::Root_Widget->Draw(nullptr);
			SwapChain->Present(0, 0);

		}
	}
	DeInit();
	SAFE_DELETE(input);// make sure to delete this here. It will call the cleanup the library in its destructor

	HR(SwapChain->SetFullscreenState(false, 0));// this is needed in case full screen is on
	RELEASECOM(BackBufferRTV);
	RELEASECOM(SwapChain);
	RELEASECOM(g_DeviceContext);
	RELEASECOM(g_Device);

}

#include "Text.h"
#include "Button.h"
#include "CheckBox.h"
#include "Radio_Group.h"
#include "Window.h"
#include "FPS.h"
#include "Text_Box.h"
#include <fstream>

MY_UI_Too::Controls::WindowTitleBar* loginwindow=nullptr;
MY_UI_Too::Controls::Text* logintxt=nullptr;
MY_UI_Too::Controls::Text_Box* logintb=nullptr;
MY_UI_Too::Controls::Text* passtxt=nullptr;
MY_UI_Too::Controls::Text_Box* password=nullptr;
MY_UI_Too::Controls::Text* validmessage=nullptr;
MY_UI_Too::Controls::Button* loginbutton=nullptr;
MY_Utilities::Signal_st<void> loginslot, checkcredentials;
MY_UI_Too::Controls::Text* Current_User=nullptr;
std::string Prev_User;
std::string Prev_PW;
//test controls
MY_UI_Too::Controls::CheckBox* check1 =nullptr;
MY_UI_Too::Controls::CheckBox* check2 =nullptr;

MY_UI_Too::Controls::Radio_Group* radiogrp=nullptr;

MY_UI_Too::Controls::Text_Box* txtbox1=nullptr;
MY_UI_Too::Controls::Text_Box* txtbox2=nullptr;

MY_UI_Too::Controls::WindowTitleBar* window=nullptr;

void Init(){

	MY_UI_Too::Controls::FPS* fps = new MY_UI_Too::Controls::FPS();
	fps->Set_Pos(MY_UI_Too::Utilities::Point(10, 10));

	MY_UI_Too::Controls::Button* chnguser = new MY_UI_Too::Controls::Button();
	chnguser->Set_Text("Change User");
	chnguser->Align_RightCenter(10, fps);
	chnguser->On_Mouse_Left_Up.Connect(&loginslot, InitLoginScreen);

	Current_User=new MY_UI_Too::Controls::Text();
	Current_User->Set_Text("Current User:None");
	Current_User->Align_RightCenter(5, chnguser);

	InitLoginScreen();
}
void InitLoginScreen(){
	DeInitLoginScreen();
	loginwindow = new MY_UI_Too::Controls::WindowTitleBar(new MY_UI_Too::Controls::Window());
	loginwindow->Set_Pos(MY_UI_Too::Utilities::Point(300, 100));
	loginwindow->Set_Title("Login Window");
	loginwindow->Set_Size(MY_UI_Too::Utilities::Point(340, 100));

	logintxt = new MY_UI_Too::Controls::Text(loginwindow);
	logintxt->Set_Text("Login:");
	logintxt->Set_Pos(MY_UI_Too::Utilities::Point(10,30));
	logintb = new MY_UI_Too::Controls::Text_Box(loginwindow);
	logintb->Align_RightCenter(5, logintxt);

	passtxt = new MY_UI_Too::Controls::Text(loginwindow);
	passtxt->Set_Text("Password:");
	passtxt->Set_Pos(MY_UI_Too::Utilities::Point(10,60));
	password = new MY_UI_Too::Controls::Text_Box(loginwindow);
	password->Align_RightCenter(5, passtxt);

	validmessage= new MY_UI_Too::Controls::Text(loginwindow);
	validmessage->Set_Text("Enter user credentials");
	validmessage->Align_BottomCenter(5, password);

	loginbutton = new MY_UI_Too::Controls::Button(loginwindow);
	loginbutton->Set_Text("Login");
	loginbutton->Align_RightCenter(10, password);
	loginbutton->On_Mouse_Left_Up.Connect(&checkcredentials, CheckCredentials);
}
void DeInitLoginScreen(){
	SaveInfo();
	MY_UI_Too::Safe_Delete(loginwindow);
	MY_UI_Too::Safe_Delete(logintxt);
	MY_UI_Too::Safe_Delete(passtxt);
	MY_UI_Too::Safe_Delete(loginbutton);
	MY_UI_Too::Safe_Delete(password);
	MY_UI_Too::Safe_Delete(logintb);
}
void DeInit(){
	MY_UI_Too::Internal::Root_Widget->DeleteAll_Children();
}
void ClearUserArea(){
	MY_UI_Too::Safe_Delete(check1);
	MY_UI_Too::Safe_Delete(check2);
	MY_UI_Too::Safe_Delete(radiogrp);
	MY_UI_Too::Safe_Delete(txtbox1);
	MY_UI_Too::Safe_Delete(txtbox2);
	MY_UI_Too::Safe_Delete(window);
}
void SaveInfo(){
	if(Prev_User.size()==0) return;
	std::ofstream file(Prev_User);
	if(!file) return;
	file<<Prev_PW<<std::endl;
	MY_UI_Too::Utilities::Point p = check1->Get_Pos();
	file<<p.x<<std::endl;
	file<<p.y<<std::endl;
	file<<check1->Get_Checked()<<std::endl;

	p = check2->Get_Pos();
	file<<p.x<<std::endl;
	file<<p.y<<std::endl;
	file<<check2->Get_Checked()<<std::endl;

	p = txtbox1->Get_Pos();
	file<<p.x<<std::endl;
	file<<p.y<<std::endl;
	file<<txtbox1->Text->Get_Text()<<std::endl;

	p = txtbox2->Get_Pos();
	file<<p.x<<std::endl;
	file<<p.y<<std::endl;
	file<<txtbox2->Text->Get_Text()<<std::endl;

	p = window->Get_Pos();
	file<<p.x<<std::endl;
	file<<p.y<<std::endl;
	file.close();
}
void LoadInfo(std::ifstream& file){
	ClearUserArea();
	int x, y;
	bool checked;
	file>>x;
	file>>y;
	file>>checked;
	check1 =new MY_UI_Too::Controls::CheckBox();
	check1->Set_Pos(MY_UI_Too::Utilities::Point(x, y));
	check1->Set_Checked(checked);

	file>>x;
	file>>y;
	file>>checked;
	check2 =new MY_UI_Too::Controls::CheckBox();
	check2->Set_Pos(MY_UI_Too::Utilities::Point(x, y));
	check2->Set_Checked(checked);

	std::string text;
	file>>x;
	file>>y;
	file>>text;

	txtbox1 = new MY_UI_Too::Controls::Text_Box();
	txtbox1->Set_Pos(MY_UI_Too::Utilities::Point(x, y));
	txtbox1->Text->Set_Text(text);

	file>>x;
	file>>y;
	file>>text;

	txtbox2 = new MY_UI_Too::Controls::Text_Box();
	txtbox2->Set_Pos(MY_UI_Too::Utilities::Point(x, y));
	txtbox2->Text->Set_Text(text);

	file>>x;
	file>>y;
	window =  new MY_UI_Too::Controls::WindowTitleBar(new MY_UI_Too::Controls::Window());
	window->Set_Pos(MY_UI_Too::Utilities::Point(x, y));
	window->Set_Title("tester");
	window->Set_Size(MY_UI_Too::Utilities::Point(200, 200));

	MY_UI_Too::Controls::Button* but = new MY_UI_Too::Controls::Button(window);
	but->Set_Pos(MY_UI_Too::Utilities::Point(20, 30));
}
void GenFile(std::ofstream& file){
	file<<200<<std::endl;
	file<<200<<std::endl;
	file<<true<<std::endl;

	file<<200<<std::endl;
	file<<240<<std::endl;
	file<<false<<std::endl;

	file<<200<<std::endl;
	file<<260<<std::endl;
	file<<"tesx1"<<std::endl;

	file<<200<<std::endl;
	file<<290<<std::endl;
	file<<"test1231"<<std::endl;

	file<<400<<std::endl;
	file<<280<<std::endl;
}


void clearlogin(){
	loginwindow->Delete_This();
	loginwindow=nullptr;
	logintxt=nullptr;
	passtxt=nullptr;
	loginbutton=nullptr;
	logintb=nullptr;
	password=nullptr;
}
void CheckCredentials(){
	std::string log = logintb->Text->Get_Text();
	std::string pw = password->Text->Get_Text();
	std::ifstream file(log);
	if(file){
		std::string pass;
		file>>pass;
		if(pass==pw){
			clearlogin();
			Current_User->Set_Text("Current User:"+ log);
			if(Prev_User.size()>0) SaveInfo();	
			
			Prev_User=log;
			Prev_PW = pw;
			LoadInfo(file);
		} else {
			validmessage->Set_Text("Incorrect login/password");
		}
		file.close();
	} else {//if file doesnt exsist create a new one
		std::ofstream f(log);
		f<<pw<<std::endl;
		clearlogin();
		Current_User->Set_Text("Current User:"+ log);
		if(Prev_User.size()>0) SaveInfo();
		Prev_User=log;
		Prev_PW = pw;
		GenFile(f);
		f.close();
		std::ifstream file(Prev_User);
		if(file) {
			file>>Prev_PW;
			LoadInfo(file);
		}

	}
}