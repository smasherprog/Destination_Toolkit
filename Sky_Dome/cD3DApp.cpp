#include "stdafx.h"
#include "cD3DApp.h"
#include "../MY_UI/cWidgetInternals.h"
#include "../Utilities/My_Timer.h"
#include "../MY_UI/DirectX11.h"
#include "../MY_UI/cWidgetBase.h"
#include "../MY_UI/cWidgetSkin.h"
#include "../MY_UI/Text.h"
#include "Sky.h"
#include "../Graphics_Lib/Base_Camera.h"
#include "../Graphics_Lib/UI_Camera.h"
#include "Sky_UI.h"
#include "../MY_UI/Window.h"
#include "../MY_UI/Slider.h"

D3DApp::D3DApp(HINSTANCE hInstance, std::string appname, unsigned int height, unsigned int width) : cBaseD3D(hInstance, appname, height, width) {

	Input = new MY_UI::Utilities::cInput();
	MY_UI::DirectX11* renderer = new MY_UI::DirectX11( Graphics::Internal::Device, Graphics::Internal::DeviceContext );

	MY_UI::cRootWidget* root = new MY_UI::cRootWidget();
	MY_UI::cWidgetSkin* skin = new MY_UI::cWidgetSkin();

	MY_UI::Init(renderer, skin, root);
	MY_UI::Utilities::SetCursor(MY_UI::Utilities::Standard);
	root->SetSize(width, height);
	Frame_Stats = new MY_UI::Controls::Text(root);
	Frame_Stats->SetPos(5, 5);
	Frame_Stats->SetColor(MY_UI::Utilities::White);


	UICamera = new UI_Camera(new Base_Camera());
	// set the camera up
	UICamera->Camera->Position = vec3(0, 50.0f, 0);
	float x, y;
	Graphics::GetBackBufferSize(x, y);

	UICamera->Camera->OnResize(y, x); 
	UICamera->Camera->SetLens(.25f*Pi, y, x , 1.0f, 15000.0f);
	sunspeed = 0.1f;
	sunangle = 90.0f;
	sundir = vec3(0, 0.0f, 0);// set the sun directly above us
	sundir.y = sinf(sunangle);
	sundir.x = cosf(sunangle);
	sundir.normalize();

	Sky_Demo = new Sky();
	Sky_Demo->Init(&sundir, &UICamera->Camera->FarPlane);

	// create the ocean
	Sky_ui = new Sky_UI();
	Sky_ui->Init(Sky_Demo);

	// add a sunspeed control to the ocean window
	MY_UI::Controls::HorizontalSlider_w_Label<float>* vsliderlab3 = new MY_UI::Controls::HorizontalSlider_w_Label<float>(Sky_ui->window);
	vsliderlab3->Widget->SetRange(0.0f, 2.0f);
	vsliderlab3->Widget->SetIncrement(.05f);
	vsliderlab3->Widget->SetSliderLength(100);
	vsliderlab3->Widget->SetLink(&sunspeed);
	vsliderlab3->SetLabel("Sun Speed", LEFT | IN_LINE, 5);
	//align to the last child in the ui window
	std::vector<MY_UI::Controls::cWidgetBase*>& child =Sky_ui->window->Internal_Widget.Children;
	//get a reference to the windows children so we can align ourselves correctly
	MY_UI::Controls::cWidgetBase* sectolast = child[child.size()-2];// vsliderlab3 was added above which inserts itself into the window
	vsliderlab3->Align_To_Widget(sectolast, BOTTOM | IN_LINE, 5);

	OUTPUT_DEBUG_MSG("Finished Loading ...");
}
D3DApp::~D3DApp(){
	SAFE_DELETE(Sky_Demo);
	SAFE_DELETE(Sky_ui);
	SAFE_DELETE(Input);
}



LRESULT D3DApp::msgProc(UINT msg, WPARAM wParam, LPARAM lParam){
	if(Input->ProcessMessage(MainWnd, msg,  wParam, lParam)) return 0;
	return cBaseD3D::msgProc(msg, wParam, lParam);
}
void D3DApp::run(){
	while(cBaseD3D::begin()){// while we should keep running
		//if( mAppPaused) Sleep(100); // lets not be a cpu hog here~!! 10 fps when there is no focus
		float clr[4] = {0, 0, 0, 1.0f};
		Graphics::Textures::RT_BackBufferTexture.ClearRenderTargets(clr);
		FrameTimer.Per_Loop();
		UICamera->Camera->PerFrame(FrameTimer.DT);

		Graphics::UnBindRenderTargets();
		Graphics::Texture* bufferRTs[] = { &Graphics::Textures::RT_BackBufferTexture };
		Graphics::BindRenderTargets(bufferRTs, 1, &Graphics::Textures::RT_DepthStencil);// set the render targets to base(aka color), normal and bind the depth texture to z checking can be done
		Graphics::Textures::RT_DepthStencil.ClearDepthTargets(true, true, 1.0f, 0);// clear the depth and set its value to 1.0f
		Graphics::Textures::RT_BackBufferTexture.ClearRenderTargets();
		Graphics::Textures::RT_Base.ClearRenderTargets();

		//update the sun
		sunangle += sunspeed * FrameTimer.DT;
		sundir.y = sinf(sunangle);
		sundir.x = cosf(sunangle);
		sundir.normalize();

		if(sunangle>Pi)	sunangle=0.0f;

		Sky_Demo->Draw(UICamera->Camera->View, UICamera->Camera->Proj);

		UpdateStats();
		MY_UI::Internal::RootWidget->Draw();
		end();
	}
}
void D3DApp::UpdateStats(){
	if(FrameTimer.Frames==0){
		std::ostringstream outs;   

		float msec(1000.0f/static_cast<float>(FrameTimer.FPS));
		outs<<" FPS: "<<FrameTimer.FPS<<" MPF: "<<msec;

		float texmem  = static_cast<float>(Graphics::Internal::TextureMemory)/1000000.0f;
		float vertexmem = static_cast<float>(Graphics::Internal::BufferMemory)/1000000.0f;
		outs<<" DrawCalls: "<<Graphics::Internal::DrawCalls <<"\nTextureMem(MB)="<<texmem<<"  VertexMem(MB)="<<vertexmem;
		Graphics::Internal::DrawCalls=0;
		FrameStats =outs.str();
		Frame_Stats->SetText(FrameStats);
	}
}
