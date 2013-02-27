#include "PCH.h"
#include "Image.h"
#include "IRenderer.h"

MY_UI_Too::Controls::Image::Image(Widget* parent): Widget(parent), _Texture(nullptr) {

}
MY_UI_Too::Controls::Image::~Image(){
	SAFE_DELETE(_Texture);
}
bool MY_UI_Too::Controls::Image::Set_Texture(std::string file){
	if(_Texture != nullptr) _Texture->clear();
	_Texture = Internal::Renderer->LoadTexture(file);
	return _Texture != nullptr;
}

void MY_UI_Too::Controls::Image::Draw(){
	Internal::Renderer->StartClip(Get_Control_Bounds());
	Internal::Renderer->DrawTexturedRect_Clip(_Texture, _UVs, Get_Control_Bounds());
	Internal::Renderer->EndClip();
}