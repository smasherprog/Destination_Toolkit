#include "PCH.h"
#include "Standard_Skin.h"
#include "IRenderer.h"


void MY_UI_Too::Standard_Skin::Init(IRenderer* renderer){
	DeInit();
	_Skin= _Renderer->CreateTexture(1024, 1024, true);
	assert(_Skin!= nullptr);
	_Down_Button_Tex = _Renderer->LoadTexture("Down_Button.png");
	assert(_Down_Button_Tex!= nullptr);
	_Textures.push_back(_Down_Button_Tex);
	_Up_Button_Tex = _Renderer->LoadTexture("Up_Button.png");
	assert(_Up_Button_Tex!= nullptr);
	_Textures.push_back(_Up_Button_Tex);
	_Hovered_Button_Tex = _Renderer->LoadTexture("Hovered_Button.png");
	assert(_Hovered_Button_Tex!= nullptr);
	_Textures.push_back(_Hovered_Button_Tex);
	Utilities::UVs uvs;
	Utilities::Rect rect;
	Utilities::Point p;

	_Renderer->Begin(_Skin);
	_Renderer->StartNewBatch();
	p= _Down_Button_Tex->Get_Dimensions();
	rect.width = p.left;
	rect.height = p.top;
	_Renderer->DrawTexturedRect_NoClip(_Down_Button_Tex, uvs, rect);

	p= _Up_Button_Tex->Get_Dimensions();
	rect.width = p.left;
	rect.height = p.top;
	_Renderer->DrawTexturedRect_NoClip(_Up_Button_Tex, uvs, rect);

	_Renderer->Draw();
	_Renderer->End();
}
void MY_UI_Too::Standard_Skin::DeInit(){
	OUTPUT_DEBUG_MSG("Shuttng down ISkin"); 
	SAFE_DELETE(_Skin); 
	for(auto beg = _Textures.begin(); beg != _Textures.end(); beg++) delete *beg; 
	_Textures.clear();
}