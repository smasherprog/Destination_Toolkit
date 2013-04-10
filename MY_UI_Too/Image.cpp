#include "PCH.h"
#include "Image.h"
#include "IRenderer.h"
#include "ISkin.h"

MY_UI_Too::Controls::Image::Image(IWidget* parent): Widget(parent), _Texture(nullptr){

}
MY_UI_Too::Controls::Image::~Image(){
	SAFE_DELETE(_Texture);
}
bool MY_UI_Too::Controls::Image::Set_Texture(std::string file, MY_UI_Too::Utilities::UVs uvs){
	_UVs= uvs;
	if(_Texture != nullptr) _Texture->clear();
	_Texture = Internal::Renderer->LoadTexture(file);
	Set_Size(_Texture->Get_Dimensions());
	return _Texture != nullptr;
}
bool MY_UI_Too::Controls::Image::Set_Texture(MY_UI_Too::Interfaces::ITexture* tex, MY_UI_Too::Utilities::UVs uvs){
	_UVs= uvs;
	SAFE_DELETE(_Texture);
	_Texture = tex;
	return true;
}
void MY_UI_Too::Controls::Image::Draw(MY_UI_Too::Interfaces::ISkin* skin){
	Internal::Renderer->DrawTexturedRect_NoClip(_Texture, _UVs, Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, _Internals.Size.x, _Internals.Size.y) );
}