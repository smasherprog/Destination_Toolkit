#include "PCH.h"
#include "Standard_Skin.h"
#include "IRenderer.h"


void MY_UI_Too::Standard_Skin::Init(Interfaces::IRenderer* renderer, unsigned int skin_size){
	DeInit();
	_Renderer = renderer;
	std::vector<Utilities::Color> trans(skin_size*skin_size);
	memset(&trans[0], 0, sizeof(Utilities::Color)*skin_size*skin_size);
	_Skin= _Renderer->CreateTexture(skin_size, skin_size, &trans[0], true);
	Utilities::Point oldscreen = _Renderer->Get_Size();//save the old size so we can restore it 
	_Renderer->Set_Size(Utilities::Point(skin_size, skin_size));
	assert(_Skin!= nullptr);

	std::vector<MY_UI_Too::Interfaces::ITexture*> textures;

	Utilities::UVs uvs;
	_Renderer->Begin(_Skin);
	_Renderer->StartNewBatch();

	textures.push_back(_Renderer->LoadTexture("Down_Button.png"));
	_Setup_Texture(textures.back(), uvs, _Down_Button);
	textures.push_back(_Renderer->LoadTexture("Up_Button.png"));
	_Setup_Texture(textures.back(), uvs, _Up_Button);
	textures.push_back(_Renderer->LoadTexture("Hovered_Button.png"));
	_Setup_Texture(textures.back(), uvs, _Hovered_Button);

	_Renderer->Draw();
	_Renderer->End();
	_Renderer->Set_Size(oldscreen);//restore the old size

	for(auto beg = textures.begin(); beg != textures.end(); beg++) assert(*beg != nullptr);
}
void MY_UI_Too::Standard_Skin::DeInit(){
	OUTPUT_DEBUG_MSG("Standard_Skin::DeInit()"); 
	SAFE_DELETE(_Skin); 
}
MY_UI_Too::Utilities::UVs MY_UI_Too::Standard_Skin::Add_To_Skin( MY_UI_Too::Interfaces::ITexture* tex) {
	Utilities::Point oldscreen = _Renderer->Get_Size();//save the old size so we can restore it 
	Utilities::UVs uvs;
	MY_UI_Too::Utilities::UVs outuvs;
	_Renderer->Begin(_Skin);
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
	assert(foundspot.x+p.x<skinsize.x);
	assert(foundspot.y+p.y<skinsize.y);
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
	rect.width=width;
	rect.height=height;
	rect+=4;

	while(true){
		bool missedall=true;
		for(size_t i=0; i< _Rects.size(); i++){
			if(_Rects[i].Intersect(rect)){
				rect.left+=width;
				if(rect.left>skinsize.x){
					rect.top+=height;
					rect.left=0;
				}
				missedall =false;
				break;
			} 
		}
		if(missedall) return Utilities::Point(rect.left, rect.top);
	} 
	return Utilities::Point(rect.left, rect.top);
}