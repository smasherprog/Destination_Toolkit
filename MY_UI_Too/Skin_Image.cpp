#include "PCH.h"
#include "Skin_Image.h"
#include "IRenderer.h"
#include "ISkin.h"

MY_UI_Too::Controls::Skin_Image::Skin_Image(IWidget* parent): Widget(parent){

}
MY_UI_Too::Controls::Skin_Image::~Skin_Image(){
}
bool MY_UI_Too::Controls::Skin_Image::Set_Texture(std::string file){
	return Set_Texture(Internal::Renderer->LoadTexture(file));
}
bool MY_UI_Too::Controls::Skin_Image::Set_Texture(MY_UI_Too::Interfaces::ITexture* tex){
	if(tex != nullptr){
		Set_Size(tex->Get_Dimensions());
		_UVs=MY_UI_Too::Internal::UI_Skin->Add_To_Skin(tex);
		SAFE_DELETE(tex);
		return true;
	}
	return false;
}
void MY_UI_Too::Controls::Skin_Image::Draw(MY_UI_Too::Interfaces::ISkin* skin){
	skin->Draw_Custom(_UVs, Utilities::Rect(_Internals.Absolute_TL.x , _Internals.Absolute_TL.y, _Internals.Size.x, _Internals.Size.y) );
}