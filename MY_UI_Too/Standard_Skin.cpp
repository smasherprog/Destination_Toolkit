#include "PCH.h"
#include "Standard_Skin.h"
#include "IRenderer.h"
#include <list>
#include "../Utilities/My_Timer.h"

#define OPTIMIZED_SEARCH

void MY_UI_Too::Standard_Skin::Init(Interfaces::IRenderer* renderer, unsigned int skin_size){
	DeInit();
	_Renderer = renderer;
	_Skin= _Renderer->CreateTexture(skin_size, skin_size, nullptr, true);

	Utilities::Point oldscreen = _Renderer->Get_Size();//save the old size so we can restore it 
	_Renderer->Set_Size(Utilities::Point(skin_size, skin_size));
	assert(_Skin!= nullptr);

	std::vector<MY_UI_Too::Interfaces::ITexture*> textures;

	Utilities::UVs uvs;
	_Renderer->Begin();
	_Renderer->Set_Render_Target(_Skin);
	_Renderer->StartNewBatch();

	textures.push_back(_Renderer->LoadTexture("Down_Button.png"));
	_Setup_Texture(textures.back(), uvs, _Down_Button);
	textures.push_back(_Renderer->LoadTexture("Button.png"));
	_Setup_Texture(textures.back(), uvs, _Up_Button);
	textures.push_back(_Renderer->LoadTexture("Hovered_Button.png"));
	_Setup_Texture(textures.back(), uvs, _Hovered_Button);

	textures.push_back(_Renderer->LoadTexture("CheckBox_Checked.png"));
	_Setup_Texture(textures.back(), uvs, _CheckBox_Checked);
	textures.push_back(_Renderer->LoadTexture("CheckBox_Checked_Hovered.png"));
	_Setup_Texture(textures.back(), uvs, _CheckBox_Checked_Hovered);
	textures.push_back(_Renderer->LoadTexture("CheckBox_UnChecked_Hovered.png"));
	_Setup_Texture(textures.back(), uvs, _CheckBox_UnChecked_Hovered);
	textures.push_back(_Renderer->LoadTexture("CheckBox_UnChecked.png"));
	_Setup_Texture(textures.back(), uvs, _CheckBox_UnChecked);

	textures.push_back(_Renderer->LoadTexture("Radio_Checked_Hovered.png"));
	_Setup_Texture(textures.back(), uvs, _Radio_Checked_Hovered);
	textures.push_back(_Renderer->LoadTexture("Radio_Checked.png"));
	_Setup_Texture(textures.back(), uvs, _Radio_Checked);
	textures.push_back(_Renderer->LoadTexture("Radio_UnChecked_Hovered.png"));
	_Setup_Texture(textures.back(), uvs, _Radio_UnChecked_Hovered);
	textures.push_back(_Renderer->LoadTexture("Radio_UnChecked.png"));
	_Setup_Texture(textures.back(), uvs, _Radio_UnChecked);

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

// the optimized search might look worse because of the use of a list and the extra work creating it; however, the unoptimized search is an N Squard problem, the the optimized is must less,
#if defined(OPTIMIZED_SEARCH)

MY_UI_Too::Utilities::Point MY_UI_Too::Standard_Skin::_Get_Pos_For_Placement(unsigned int width, unsigned int height){
	Utilities::Point skinsize = _Skin->Get_Dimensions();
	Utilities::Rect rect;
	rect.width=width+4;//add a little padding
	rect.height=height+4;//add a little padding

	std::list<Utilities::Rect> copy_rects;
	for(size_t i=0; i< _Rects.size(); i++) copy_rects.push_back(_Rects[i]);

	My_Timer::tick_count timer = My_Timer::tick_count::now();
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
		if(missedall){
			//std::cout<<"time taken: "<<(My_Timer::tick_count::now() - timer).micro_seconds()<<std::endl;
			return Utilities::Point(rect.left, rect.top);
		}
	} 
	return Utilities::Point(rect.left, rect.top);
}
#else

MY_UI_Too::Utilities::Point MY_UI_Too::Standard_Skin::_Get_Pos_For_Placement(unsigned int width, unsigned int height){
	Utilities::Point skinsize = _Skin->Get_Dimensions();
	Utilities::Rect rect;
	rect.width=width+4;//add a little padding
	rect.height=height+4;//add a little padding

	My_Timer::tick_count timer = My_Timer::tick_count::now();

	while(true){
		bool missedall=true;
		for(size_t i=0; i< _Rects.size(); i++){
			if(_Rects[i].Intersect(rect)){
				rect.left+=width;
				if(rect.left>=skinsize.x){
					rect.top+=height;
					rect.left=0;
				}
				missedall =false;
				break;
			} else {
				if(rect.left + (int)width>=skinsize.x){
					rect.top+=height +4;
					rect.left=0;
					missedall =false;
					break;
				}
			}
		}
		if(missedall){
			//std::cout<<"time taken: "<<(My_Timer::tick_count::now() - timer).micro_seconds()<<std::endl;
			return Utilities::Point(rect.left, rect.top);
		}
	} 
	return Utilities::Point(rect.left, rect.top);
}

#endif

