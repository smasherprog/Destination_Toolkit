#include "PCH.h"
#include "Standard_Skin.h"
#include "IRenderer.h"
#include <list>
#include "../Utilities/My_Timer.h"

#define BORDER_SIZE 10 //in pixels

void MY_UI_Too::Standard_Skin::Init(Interfaces::IRenderer* renderer, unsigned int skin_size){
	DeInit();
	_Renderer = renderer;
	_Skin= _Renderer->CreateTexture(skin_size, skin_size, nullptr, true);
	_Border_Size = (float)BORDER_SIZE/(float)skin_size;
	Utilities::Point oldscreen = _Renderer->Get_Size();//save the old size so we can restore it 
	_Renderer->Set_Size(Utilities::Point(skin_size, skin_size));
	assert(_Skin!= nullptr);

	std::vector<MY_UI_Too::Interfaces::ITexture*> textures;

	Utilities::UVs uvs;
	_Renderer->Begin();
	_Renderer->Set_Render_Target(_Skin);
	_Renderer->StartNewBatch();
	std::string basefolder= "../Assets/UI_Textures/";
	textures.push_back(_Renderer->LoadTexture(basefolder+"Down_Button.png"));
	_Setup_Texture(textures.back(), uvs, _Down_Button);
	textures.push_back(_Renderer->LoadTexture(basefolder+"Up_Button.png"));
	_Setup_Texture(textures.back(), uvs, _Up_Button);
	textures.push_back(_Renderer->LoadTexture(basefolder+"Hovered_Button.png"));
	_Setup_Texture(textures.back(), uvs, _Hovered_Button);

	textures.push_back(_Renderer->LoadTexture(basefolder+"CheckBox_Checked.png"));
	_Setup_Texture(textures.back(), uvs, _CheckBox_Checked);
	textures.push_back(_Renderer->LoadTexture(basefolder+"CheckBox_Checked_Hovered.png"));
	_Setup_Texture(textures.back(), uvs, _CheckBox_Checked_Hovered);
	textures.push_back(_Renderer->LoadTexture(basefolder+"CheckBox_UnChecked_Hovered.png"));
	_Setup_Texture(textures.back(), uvs, _CheckBox_UnChecked_Hovered);
	textures.push_back(_Renderer->LoadTexture(basefolder+"CheckBox_UnChecked.png"));
	_Setup_Texture(textures.back(), uvs, _CheckBox_UnChecked);

	textures.push_back(_Renderer->LoadTexture(basefolder+"Radio_Checked_Hovered.png"));
	_Setup_Texture(textures.back(), uvs, _Radio_Checked_Hovered);
	textures.push_back(_Renderer->LoadTexture(basefolder+"Radio_Checked.png"));
	_Setup_Texture(textures.back(), uvs, _Radio_Checked);
	textures.push_back(_Renderer->LoadTexture(basefolder+"Radio_UnChecked_Hovered.png"));
	_Setup_Texture(textures.back(), uvs, _Radio_UnChecked_Hovered);
	textures.push_back(_Renderer->LoadTexture(basefolder+"Radio_UnChecked.png"));
	_Setup_Texture(textures.back(), uvs, _Radio_UnChecked);

	textures.push_back(_Renderer->LoadTexture(basefolder+"Window_Hovered_Close_Button.png"));
	_Setup_Texture(textures.back(), uvs, _Window_Hovered_Close_Button);
	textures.push_back(_Renderer->LoadTexture(basefolder+"Window_Close_Button.png"));
	_Setup_Texture(textures.back(), uvs, _Window_Close_Button);

	textures.push_back(_Renderer->LoadTexture(basefolder+"Window_Top.png"));
	_Wnd_Title_Height = textures.back()->Get_Dimensions().y;
	_Setup_Texture(textures.back(), uvs, _Window_Top);
	textures.push_back(_Renderer->LoadTexture(basefolder+"Window_Bottom.png"));
	_Setup_Texture(textures.back(), uvs, _Window_Bottom);

	textures.push_back(_Renderer->LoadTexture(basefolder+"Text_Box_Focus.png"));
	_Setup_Texture(textures.back(), uvs, _Text_Box_Focus);
	textures.push_back(_Renderer->LoadTexture(basefolder+"Text_Box_No_Focus.png"));
	_Setup_Texture(textures.back(), uvs, _Text_Box_No_Focus);


	_Renderer->Draw();
	_Renderer->End();
	_Renderer->Set_Size(oldscreen);//restore the old size

	for(auto beg = textures.begin(); beg != textures.end(); beg++) {
		assert(*beg != nullptr);
		delete *beg;//textures arent needed any more
	}
}
void MY_UI_Too::Standard_Skin::DeInit(){
	OUTPUT_DEBUG_MSG("Standard_Skin::DeInit()"); 
	SAFE_DELETE(_Skin); 
}
void MY_UI_Too::Standard_Skin::Remove_From_Skin(Utilities::UVs u){
	Utilities::Point skinsize = _Skin->Get_Dimensions();
	Utilities::Rect t;
	t.left =  (int)(u.u1*skinsize.x);
	t.width = (int)(u.u2*skinsize.x) - t.left;
	t.top =  (int)(u.v1*skinsize.y);
	t.height =  (int)(u.v2*skinsize.y) - t.top;
	// since the conversion from float to int might not be exact, the comparison is +-1 on all points. 
	for(auto beg = _Rects.begin(); beg!= _Rects.end(); beg++){
		if(beg->left >= t.left -1 && beg->left <= t.left +1
			&& beg->top >= t.top -1 && beg->top <= t.top +1
			&& beg->height >= t.height -1 && beg->height <= t.height +1
			&& beg->width >= t.width -1 && beg->width <= t.width +1){
				_Rects.erase(beg);
				return;
		}
	}
}
MY_UI_Too::Utilities::UVs MY_UI_Too::Standard_Skin::Add_To_Skin( MY_UI_Too::Interfaces::ITexture* tex) {
	Utilities::Point oldscreen = _Renderer->Get_Size();//save the old size so we can restore it 
	Utilities::UVs uvs;
	Utilities::Point skinsize = _Skin->Get_Dimensions();//save the old size so we can restore it 
	_Renderer->Set_Size(skinsize);
	MY_UI_Too::Utilities::UVs outuvs;
	_Renderer->Begin();
	_Renderer->Set_Render_Target(_Skin);
	_Renderer->StartNewBatch();
	_Setup_Texture(tex, uvs, outuvs);

	_Renderer->Draw();
	_Renderer->End();
	_Renderer->Set_Size(oldscreen);//restore the old size
	return outuvs;
}

void MY_UI_Too::Standard_Skin::_Setup_Texture(Interfaces::ITexture* tex, Utilities::UVs in_uvs, Utilities::UVs& out_uvs){
	Utilities::Point p= tex->Get_Dimensions();
	Utilities::Point skinsize =_Skin->Get_Dimensions();
	Utilities::Point foundspot = _Get_Pos_For_Placement(p.x, p.y);
	assert(foundspot.x+p.x<=skinsize.x);
	assert(foundspot.y+p.y<=skinsize.y);
	Utilities::Rect rect;
	rect.left=foundspot.x;
	rect.top = foundspot.y;
	rect.width=p.x;
	rect.height=p.y;
	_Rects.push_back(rect);

	_Renderer->DrawTexturedRect_NoClip(tex, in_uvs, rect);
	out_uvs.u1 = (float)rect.left;
	out_uvs.u1/= (float)skinsize.x;
	out_uvs.v1 = (float)rect.top;
	out_uvs.v1/= (float)skinsize.y;

	out_uvs.u2 = (float)(rect.left+ rect.width);
	out_uvs.u2/= (float)skinsize.x;
	out_uvs.v2 = (float)(rect.top + rect.height);
	out_uvs.v2/= (float)skinsize.y;
}

MY_UI_Too::Utilities::Point MY_UI_Too::Standard_Skin::_Get_Pos_For_Placement(unsigned int width, unsigned int height){
	Utilities::Point skinsize = _Skin->Get_Dimensions();
	Utilities::Rect rect;
	rect.width=width+1;//add a little padding
	rect.height=height+1;//add a little padding

	std::list<Utilities::Rect> copy_rects;
	for(size_t i=0; i< _Rects.size(); i++) copy_rects.push_back(_Rects[i]);


	while(true){
		bool missedall=true;
		for(auto beg = copy_rects.begin(); beg!= copy_rects.end(); beg++){
			Utilities::Rect& test_rect = *beg;
			if(test_rect.Intersect(rect)){
				//test to see if the copy_rects[i] is equal to or less than rect. If so, I can remove it from the check because it will be passed on the next line
				int test_right = test_rect.left + test_rect.width;
				int test_bottom = test_rect.top + test_rect.height;
				int rect_right = rect.left + rect.width;
				int rect_bottom = rect.top + rect.height;

				rect.left+=test_rect.width+1;//move passed the object just hit
				if(rect.left + rect.width>=skinsize.x){
					rect.top+=rect.height+1;
					rect.left=0;
				}
				if((test_right <= rect_right) & (test_bottom<= rect_bottom)) beg = copy_rects.erase(beg);// ADVANCE
				missedall =false;
				break;
			} else {
				if(rect.left + rect.width>=skinsize.x){
					rect.top+=rect.height+1;
					rect.left=0;
					missedall =false;
					break;
				}
			}
		}
		if(missedall) return Utilities::Point(rect.left, rect.top);

	} 
	return Utilities::Point(rect.left, rect.top);
}

void MY_UI_Too::Standard_Skin::Draw_Button(Widget_States state, MY_UI_Too::Utilities::Rect bounds) const{
	Utilities::UVs uv;

	if(state == Widget_States::Hovered) uv = _Hovered_Button;
	else if(state == Widget_States::Pressed) uv = _Down_Button;
	else uv = _Up_Button;
	_Renderer->DrawTexturedRect_NoClip(_Skin, uv, bounds);

}
void MY_UI_Too::Standard_Skin::Draw_CheckBox(Widget_States state, bool checked, MY_UI_Too::Utilities::Rect bounds) const{
	Utilities::UVs uv;
	if(checked){
		if(state == Widget_States::Hovered) uv = _CheckBox_Checked_Hovered;
		else if(state == Widget_States::Pressed) uv = _CheckBox_Checked_Hovered;
		else uv = _CheckBox_Checked;
	} else {
		if(state == Widget_States::Hovered) uv = _CheckBox_UnChecked_Hovered;
		else if(state == Widget_States::Pressed) uv = _CheckBox_UnChecked_Hovered;
		else uv = _CheckBox_UnChecked;
	}
	_Renderer->DrawTexturedRect_NoClip(_Skin, uv, bounds);
}
void MY_UI_Too::Standard_Skin::Draw_Radio(Widget_States state,bool checked, MY_UI_Too::Utilities::Rect bounds) const{
	Utilities::UVs uv;
	if(checked){
		if(state == Widget_States::Hovered) uv = _Radio_Checked_Hovered;
		else if(state == Widget_States::Pressed) uv = _Radio_Checked_Hovered;
		else uv = _Radio_Checked;
	} else {
		if(state == Widget_States::Hovered) uv = _Radio_UnChecked_Hovered;
		else if(state == Widget_States::Pressed) uv = _Radio_UnChecked_Hovered;
		else uv = _Radio_UnChecked;
	}
	_Renderer->DrawTexturedRect_NoClip(_Skin, uv, bounds);
}
void MY_UI_Too::Standard_Skin::Draw_Window_BackGround(Widget_States state, MY_UI_Too::Utilities::Rect bounds) const{

	MY_UI_Too::Utilities::UVs uvs = _Window_Top;
	MY_UI_Too::Utilities::Rect r(bounds);
	
	//now the top left corner
	r.top = bounds.top;
	r.left = bounds.left;
	r.height = BORDER_SIZE;
	r.width =  BORDER_SIZE;

	uvs = _Window_Bottom;
	uvs.u1 = _Window_Bottom.u1;
	uvs.u2 = _Window_Bottom.u1 +_Border_Size;
	uvs.v1 = _Window_Bottom.v1;
	uvs.v2 = _Window_Bottom.v1 +_Border_Size;
	_Renderer->DrawTexturedRect_NoClip(_Skin, uvs, r);

	//now the top center
	r.top = bounds.top;
	r.left = bounds.left + BORDER_SIZE;
	r.height = BORDER_SIZE;
	r.width =  bounds.width - (BORDER_SIZE*2);

	uvs = _Window_Bottom;
	uvs.u1 = _Window_Bottom.u1 +_Border_Size;
	uvs.u2 = _Window_Bottom.u2 -_Border_Size;
	uvs.v1 = _Window_Bottom.v1;
	uvs.v2 = _Window_Bottom.v1 +_Border_Size;
	_Renderer->DrawTexturedRect_NoClip(_Skin, uvs, r);

	//now the top right
	r.top = bounds.top;
	r.left = bounds.left + bounds.width - BORDER_SIZE;
	r.height = BORDER_SIZE;
	r.width =  BORDER_SIZE;

	uvs = _Window_Bottom;
	uvs.u1 = _Window_Bottom.u2 - _Border_Size;
	uvs.u2 = _Window_Bottom.u2;
	uvs.v1 = _Window_Bottom.v1;
	uvs.v2 = _Window_Bottom.v1 +_Border_Size;
	_Renderer->DrawTexturedRect_NoClip(_Skin, uvs, r);

	//now the right center
	r.top = bounds.top+ BORDER_SIZE;
	r.left = bounds.left + bounds.width - BORDER_SIZE;
	r.height = bounds.height - (BORDER_SIZE*2);
	r.width =  BORDER_SIZE;

	uvs = _Window_Bottom;
	uvs.u1 = _Window_Bottom.u2 - _Border_Size;
	uvs.u2 = _Window_Bottom.u2;
	uvs.v1 = _Window_Bottom.v1 + _Border_Size;
	uvs.v2 = _Window_Bottom.v2 - _Border_Size;
	_Renderer->DrawTexturedRect_NoClip(_Skin, uvs, r);

		//now the right bottom corner
	r.top = bounds.top + bounds.height- BORDER_SIZE;
	r.left = bounds.left + bounds.width - BORDER_SIZE;
	r.height = BORDER_SIZE;
	r.width =  BORDER_SIZE;

	uvs = _Window_Bottom;
	uvs.u1 = _Window_Bottom.u2 - _Border_Size;
	uvs.u2 = _Window_Bottom.u2;
	uvs.v1 = _Window_Bottom.v2 - _Border_Size;
	uvs.v2 = _Window_Bottom.v2;
	_Renderer->DrawTexturedRect_NoClip(_Skin, uvs, r);

	//now the bottom center
	r.top = bounds.top + bounds.height- BORDER_SIZE;
	r.left = bounds.left + BORDER_SIZE;
	r.height = BORDER_SIZE;
	r.width = bounds.width - (BORDER_SIZE*2);

	uvs = _Window_Bottom;
	uvs.u1 = _Window_Bottom.u1 + _Border_Size;
	uvs.u2 = _Window_Bottom.u2 - _Border_Size;
	uvs.v1 = _Window_Bottom.v2 - _Border_Size;
	uvs.v2 = _Window_Bottom.v2;
	_Renderer->DrawTexturedRect_NoClip(_Skin, uvs, r);

	//now the bottom left corner
	r.top = bounds.top + bounds.height- BORDER_SIZE;
	r.left = bounds.left;
	r.height = BORDER_SIZE;
	r.width = BORDER_SIZE;

	uvs = _Window_Bottom;
	uvs.u1 = _Window_Bottom.u1;
	uvs.u2 = _Window_Bottom.u1 + _Border_Size;
	uvs.v1 = _Window_Bottom.v2 - _Border_Size;
	uvs.v2 = _Window_Bottom.v2;
	_Renderer->DrawTexturedRect_NoClip(_Skin, uvs, r);

	//now the left center
	r.top = bounds.top + BORDER_SIZE;
	r.left = bounds.left;
	r.height = bounds.height - (BORDER_SIZE*2);
	r.width = BORDER_SIZE;

	uvs = _Window_Bottom;
	uvs.u1 = _Window_Bottom.u1;
	uvs.u2 = _Window_Bottom.u1 + _Border_Size;
	uvs.v1 = _Window_Bottom.v1 + _Border_Size;
	uvs.v2 = _Window_Bottom.v2 - _Border_Size;
	_Renderer->DrawTexturedRect_NoClip(_Skin, uvs, r);
}
void MY_UI_Too::Standard_Skin::Draw_Window_TitleBar(Widget_States state, MY_UI_Too::Utilities::Rect bounds) const{

	MY_UI_Too::Utilities::UVs uvs = _Window_Top;
	MY_UI_Too::Utilities::Rect r(bounds);
	r.width -=BORDER_SIZE;
	r.left += BORDER_SIZE/2;
	r.height = _Wnd_Title_Height;
	uvs *= _Border_Size;
	_Renderer->DrawTexturedRect_NoClip(_Skin, _Window_Top, r);

	uvs = _Window_Top;
	uvs.u2 = uvs.u1 + _Border_Size;
	r.left = bounds.left;
	r.width = BORDER_SIZE;
	_Renderer->DrawTexturedRect_NoClip(_Skin, uvs, r);

	uvs.u1 =_Window_Top.u2 - _Border_Size;
	uvs.u2 = _Window_Top.u2;
	r.width = BORDER_SIZE;
	r.left = (bounds.left + bounds.width) - BORDER_SIZE;
	_Renderer->DrawTexturedRect_NoClip(_Skin, uvs, r);

	//draw the center 
	r = bounds;
	r.left += BORDER_SIZE;
	r.height = bounds.height - _Wnd_Title_Height - (BORDER_SIZE*2);
	r.width =  bounds.width - (BORDER_SIZE*2);
	r.top += _Wnd_Title_Height + BORDER_SIZE;
	uvs = _Window_Bottom;
	uvs.u1 += _Border_Size;
	uvs.u2 -= _Border_Size;
	uvs.v1 += _Border_Size;
	uvs.v2 -= _Border_Size;
	_Renderer->DrawTexturedRect_NoClip(_Skin, uvs, r);

}
void MY_UI_Too::Standard_Skin::Draw_WindowCloseButton(Widget_States state, MY_UI_Too::Utilities::Rect bounds) const{
	Utilities::UVs uv;
	if(state == Widget_States::Hovered || state == Widget_States::Pressed) uv = _Window_Hovered_Close_Button;
	else uv = _Window_Close_Button;
	_Renderer->DrawTexturedRect_NoClip(_Skin, uv, bounds);
}
void MY_UI_Too::Standard_Skin::Draw_Text_Box(bool focus, MY_UI_Too::Utilities::Rect bounds) const{
	if(focus)  _Renderer->DrawTexturedRect_NoClip(_Skin, _Text_Box_Focus, bounds);
	else _Renderer->DrawTexturedRect_NoClip(_Skin, _Text_Box_No_Focus, bounds);
}
void MY_UI_Too::Standard_Skin::Draw_Custom(MY_UI_Too::Utilities::UVs uvs, MY_UI_Too::Utilities::Rect bounds) const{
	_Renderer->DrawTexturedRect_NoClip(_Skin, uvs, bounds);
}

void MY_UI_Too::Standard_Skin::Draw_Text(MY_UI_Too::Interfaces::IFont* font, std::string txt, MY_UI_Too::Utilities::Point startinpos, unsigned int fontsize,  MY_UI_Too::Utilities::Color color) const{
	_Renderer->DrawText_NoClip(_Skin, font, txt, startinpos, fontsize, color, color, color, color);
}
void MY_UI_Too::Standard_Skin::Draw_Caret(MY_UI_Too::Interfaces::IFont* font, std::string txt, MY_UI_Too::Utilities::Point startinpos, unsigned int fontsize, unsigned int carentindex,  MY_UI_Too::Utilities::Color color) const{
	_Renderer->DrawTextCaret_NoClip(_Skin, font, txt, startinpos, fontsize, carentindex, color, color, color, color);
}

MY_UI_Too::Utilities::Point MY_UI_Too::Standard_Skin::Measure_String(MY_UI_Too::Interfaces::IFont* font, unsigned int fontsize, std::string text) const{
	return _Renderer->Measure_String(_Skin->Get_Dimensions(), font, fontsize, text);
}