#include "PCH.h"
#include "Text.h"
#include "IFont_Factory.h"
#include "IFont.h"
#include "ISkin.h"

MY_UI_Too::Controls::Text::Text(IWidget* widget): Widget(widget){
	_Font = Internal::Font_Factory->Get_Font();
	_Font_Size=14;//defaults
	Set_Color(Utilities::Black);
	_Bold=false;

}
MY_UI_Too::Controls::Text::~Text(){
	if(Internal::Font_Factory!= nullptr) Internal::Font_Factory->Destroy_Font(_Font->Get_Font());
}

void MY_UI_Too::Controls::Text::Draw(MY_UI_Too::Interfaces::ISkin* skin){
	skin->Draw_Text(_Font, _Text, _Internals.Absolute_TL, _Font_Size);
	if(_Bold) skin->Draw_Text(_Font, _Text, _Internals.Absolute_TL+ Utilities::Point(1, 1), _Font_Size);//bold just adds a little thickness
}
void MY_UI_Too::Controls::Text::Draw_Caret(MY_UI_Too::Interfaces::ISkin* skin, unsigned int caretindex){
	skin->Draw_Caret(_Font, _Text, _Internals.Absolute_TL, _Font_Size, caretindex);
}

void MY_UI_Too::Controls::Text::Set_Font(std::string fontname){
	_Font = Internal::Font_Factory->Get_Font(fontname);
	if(_Font == nullptr){
		OUTPUT_DEBUG_MSG("Cannot find the font "<<fontname);
		_Font = Internal::Font_Factory->Get_Font();
	}
}
void MY_UI_Too::Controls::Text::Set_Bold(bool bold){
	_Bold=bold;
}
bool MY_UI_Too::Controls::Text::Get_Bold(){
	return _Bold;
}
std::string MY_UI_Too::Controls::Text::Get_Font() const{
	return _Font->Get_Font();
}
void MY_UI_Too::Controls::Text::Set_Font_Size(unsigned int size){
	_Font_Size=size;
}
unsigned int MY_UI_Too::Controls::Text::Get_Font_Size() const{
	return _Font_Size;
}
void MY_UI_Too::Controls::Text::Set_Text(std::string text){
	_Text = text;
	Utilities::Point p = Internal::UI_Skin->Measure_String( _Font, _Font_Size, text);
	if(p.y==0.0f) p.y = _Font_Size;
	Set_Size(p);
}
std::string MY_UI_Too::Controls::Text::Get_Text() const{
	return _Text;
}