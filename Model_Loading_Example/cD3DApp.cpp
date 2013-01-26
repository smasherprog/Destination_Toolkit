#include "stdafx.h"
#include "cD3DApp.h"
#include "../MY_UI/cWidgetInternals.h"
#include "../Utilities/My_Timer.h"
#include "../MY_UI/DirectX11.h"
#include "../MY_UI/cWidgetBase.h"
#include "../MY_UI/cWidgetSkin.h"
#include "../MY_UI/Text.h"
#include "../Graphics_Lib/UI_Camera.h"
#include "../Graphics_Lib/Base_Mesh.h"
#include "Mesh_UI.h"
#include "../assimp/include/assimp/cimport.h"
#include "../Graphics_Lib/Static_Mesh.h"

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
	UICamera->Camera->Position = vec3(0, 0.0f, -170);
	float x, y;
	Graphics::GetBackBufferSize(x, y);

	UICamera->Camera->OnResize(y, x); 
	UICamera->Camera->SetLens(.25f*Pi, y, x , 1.0f, 1500.0f);

	MeshUI = new Mesh_UI();//pass the add mesh function. The UI does not manage the lifetime, just acts as a middle man
	MeshUI->Set_AddMesh_Func(std::bind(&D3DApp::AddMesh, this, std::placeholders::_1));
	aiString sz;
	aiGetExtensionList(&sz);
	MeshUI->SetFileExts(std::string(sz.C_Str()));
}
D3DApp::~D3DApp(){
	for(size_t i =0 ; i< Mesh_Container.size(); i++) delete Mesh_Container[i];

	SAFE_DELETE(Input);
}
void D3DApp::AddMesh(Base_Mesh* m){
	Mesh_Container.push_back(m);
}


LRESULT D3DApp::msgProc(UINT msg, WPARAM wParam, LPARAM lParam){
	if(Input->ProcessMessage(MainWnd, msg,  wParam, lParam)) return 0;
	return cBaseD3D::msgProc(msg, wParam, lParam);
}
void D3DApp::run(){
	while(cBaseD3D::begin()){// while we should keep running
		Graphics::UnBindRenderTargets();
		Graphics::Texture* bufferRTs[] = { &Graphics::Textures::RT_BackBufferTexture };
		Graphics::BindRenderTargets(bufferRTs, 1, &Graphics::Textures::RT_DepthStencil);// set the render targets to base(aka color), normal and bind the depth texture to z checking can be done
		Graphics::Textures::RT_DepthStencil.ClearDepthTargets(true, true, 1.0f, 0);// clear the depth and set its value to 1.0f
		Graphics::Textures::RT_BackBufferTexture.ClearRenderTargets();

		FrameTimer.Per_Loop();
		UICamera->Camera->PerFrame(FrameTimer.DT);

		for(size_t i =0 ; i< Mesh_Container.size(); i++) {
			Mesh_Container[i]->Draw(UICamera->Camera->View, UICamera->Camera->Proj);
		}
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
