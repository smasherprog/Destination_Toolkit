#include "stdafx.h"

#include "Application.h"
#include "../MY_UI/cWidgetInternals.h"
#include "../MY_UI/DirectX11.h"
#include "../MY_UI/cWidgetBase.h"
#include "../MY_UI/cWidgetSkin.h"
#include "../MY_UI/Text.h"
#include "../Utilities/My_Timer.h"
#include "../Graphics_Lib/UI_Camera.h"
#include "../Graphics_Lib/cCamera.h"
#include <sstream>
#include "Simple_Terrain.h"

Application::Application(HINSTANCE hInstance, std::string appname, unsigned int height, unsigned int width) : cBaseD3D(hInstance, appname, height, width) {
// the next few lines are the standard MY_UI setup . . 
	Input = new MY_UI::Utilities::cInput();
	MY_UI::DirectX11* renderer = new MY_UI::DirectX11(Graphics::Internal::Device, Graphics::Internal::DeviceContext );
	MY_UI::cRootWidget* root = new MY_UI::cRootWidget();
	MY_UI::cWidgetSkin* skin = new MY_UI::cWidgetSkin();
	MY_UI::Init(renderer, skin, root);
	MY_UI::Utilities::SetCursor(MY_UI::Utilities::Standard);
	root->SetSize(width, height);
//End MY_UI setup...

	Frame_Stats = new MY_UI::Controls::Text(root);
	Frame_Stats->SetPos(5, 5);
	Frame_Stats->SetColor(MY_UI::Utilities::White);
	
//initialize camera
	Camera.Position = vec3(0, 50.0f, 0);// set the camera above the terrain a little
	float x, y;
	Graphics::GetBackBufferSize(x, y);
	Camera.setLens(.25f*Pi, y, x , 1.0f, 5000.0f);
	Camera.rebuildView(); 

	UI_Camera::self = new UI_Camera();

	Terrain = new Simple_Terrain();
	Terrain->Init();
	
}
Application::~Application(){
	delete Terrain;
	// deleting the Input will clean up the entire library no need to free any of the other memory
	SAFE_DELETE(Input);
}

LRESULT Application::msgProc(UINT msg, WPARAM wParam, LPARAM lParam){
	if(Input->ProcessMessage(MainWnd, msg,  wParam, lParam)) return 0;
	return cBaseD3D::msgProc(msg, wParam, lParam);
}

void Application::run(){
	while(cBaseD3D::begin()){// while we should keep running
		Camera.rebuildView();//update the camera
		//if( mAppPaused) Sleep(100); // lets not be a cpu hog here~!! 10 fps when there is no focus
		float clr[4] = {0, 0, 0, 1.0f};
		Graphics::UnBindRenderTargets();
		Graphics::Texture* bufferRTs[] = { &Graphics::Textures::RT_BackBufferTexture, &Graphics::Textures::RT_Normal, &Graphics::Textures::RT_Depth };
		Graphics::BindRenderTargets(bufferRTs, 3, &Graphics::Textures::RT_DepthStencil);// set the render targets to base(aka color), normal and bind the depth texture to z checking can be done
		Graphics::Textures::RT_DepthStencil.ClearDepthTargets(true, true, 1.0f, 0);// clear the depth and set its value to 1.0f
		//Graphics::Textures::RT_BackBufferTexture.ClearRenderTargets();
		Graphics::Textures::RT_Base.ClearRenderTargets();
		My_Timer::Run();// update the game timer..... You can get DT from here as well as random numbers
		/*
		 *
		 INSERT DRAWING CODE HERE!!!!!
		 
		 *
		 */

		Terrain->Draw();
		UpdateStats();
		MY_UI::Internal::RootWidget->Draw();// draw the UI at the very end
		end();
		if(Current_Key == KEY_ESCAPE) break;// if esc is pressed, shut down the application
	}
}
void Application::UpdateStats(){
	static unsigned int timer =0;
	timer +=GetTickCount();
	if(timer > 1000){// only update the frame stats every second. A change in the UI will cause the entire UI to be rebuilt. So, dont update the frame stats each frame..
		std::ostringstream outs;   
		outs.precision(4);

		float msec(1000.0f/static_cast<float>(Graphics::Internal::FramesPerSecond));
		outs<<" FPS: "<<Graphics::Internal::FramesPerSecond<<" MPF: "<<msec;
		float texmem  = static_cast<float>(Graphics::Internal::TextureMemory)/1000000.0f;
		float vertexmem = static_cast<float>(Graphics::Internal::BufferMemory)/1000000.0f;
		outs<<" DrawCalls: "<<Graphics::Internal::DrawCalls <<"\nTextureMem(MB)="<<texmem<<"  VertexMem(MB)="<<vertexmem;
		Graphics::Internal::DrawCalls=0;
		FrameStats =outs.str();
		Frame_Stats->SetText(FrameStats);
		timer=0;
	}
}
