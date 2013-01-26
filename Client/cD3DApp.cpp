#include "stdafx.h"
#include "cD3DApp.h"
#include "../MY_UI/cWidgetInternals.h"
#include "../Utilities/My_Timer.h"
#include "../MY_UI/DirectX11.h"
#include "../MY_UI/cWidgetBase.h"
#include "../MY_UI/cWidgetSkin.h"
#include "../MY_UI/Text.h"
#include "cLoginControl.h"
#include "cDemoControl.h"

D3DApp::D3DApp(HINSTANCE hInstance, std::string appname, unsigned int height, unsigned int width) : cBaseD3D(hInstance, appname, height, width) {

	LoginControl =0;
	DemoControl =0;
	My_Timer::RandomInit(GetTickCount());

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

	LoginControl = new cLoginControl();
}
D3DApp::~D3DApp(){
	SAFE_DELETE(LoginControl);
	SAFE_DELETE(DemoControl);
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
		My_Timer::Run();// update the game timer.....
		if(DemoControl){
			if(DemoControl->Focus == Stopped ) {
				SAFE_DELETE(DemoControl);
				SAFE_DELETE(LoginControl);// just in case
				LoginControl = new cLoginControl();
			}
			else DemoControl->Run();
		}

		if(LoginControl){
			if(LoginControl->Focus == Stopped ) {
				SAFE_DELETE(DemoControl);
				DemoControl = new cDemoControl();
				DemoControl->Init(LoginControl->GameServerIP, LoginControl->Login, (uint8_t*)&LoginControl->Key[0]);
				SAFE_DELETE(LoginControl);// just in case
			} else if(LoginControl->Focus == ShutDownAll ){
				SAFE_DELETE(DemoControl);
				SAFE_DELETE(LoginControl);
			}
			else LoginControl->Run();
		}

		UpdateStats();
		MY_UI::Internal::RootWidget->Draw();
		end();
		if(!DemoControl & !LoginControl) PostQuitMessage(0);
	}
}
void D3DApp::UpdateStats(){
	static unsigned int timer =0;
	timer +=GetTickCount();
	if(timer > 1000){
		ostringstream outs;   
		outs.precision(4);
		if(DemoControl){
			if(DemoControl->Peer){
				int32_t x(static_cast<int32_t>(Camera.Position.x)), y(static_cast<int32_t>(Camera.Position.y)),  z(static_cast<int32_t>(Camera.Position.z));
				//			outs<<"\nPosition("<<x<<" , "<<y<<" , "<<z<<")"<<"\nTriangles Drawn = "<<Graphics::Internal::TrianglesDrawn<<"LightsDrawn = "<<DemoControl.ObjectContainer.PointLightsDrawn;
				if(DemoControl->Peer)
					outs<<endl<<"Down: "<<DemoControl->Peer->BytesReceivedPerSecond<<" bps, Up: "<<DemoControl->Peer->BytesSentPerSecond<<" bps. Loss: "<<DemoControl->Peer->CalculateLoss()<<"%, Ping: "<<DemoControl->Peer->GetAveragePing(50)<<"ms";
			}
		} else if(LoginControl){
			if(LoginControl->Peer){
				outs<<endl<<"Down: "<<LoginControl->Peer->BytesReceivedPerSecond<<" bps, Up: "<<LoginControl->Peer->BytesSentPerSecond<<" bps. Loss: "<<LoginControl->Peer->CalculateLoss()<<"%, Ping: "<<LoginControl->Peer->GetAveragePing(100)<<"ms";
			}
		}

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
