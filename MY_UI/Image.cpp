#include "PCH.h"
#include "Image.h"
#include "cWidgetRenderer.h"
#include "cWidgetSkin.h"


MY_UI::Controls::Image::~Image(){
	ClearImages();
}

void MY_UI::Controls::Image::Draw_NoClip(){
	if(Internal_Image.ImageShown != -1){// only draw an image if there is an image to be shown
		if(Internal_Widget.Hidden) return;
		
		Utilities::Rect r =Internal_Widget.AbsolutePosition;
		MY_UI::Utilities::Texture& tex = Internal_Image.Textures[Internal_Image.ImageShown].Texture;
		auto& t = Internal_Image.Textures[Internal_Image.ImageShown];
		if(t.DrawBorder){
			// top rect
			r.height =t.BorderYSize;
			Internal::WidgetRenderer->DrawTexturedRect_NoClip(tex, t.Top, r, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
			// bottom rect
			r.top = Internal_Widget.AbsolutePosition.top + Internal_Widget.AbsolutePosition.height -t.BorderYSize;
	
			Internal::WidgetRenderer->DrawTexturedRect_NoClip(tex, t.Bottom, r, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
			// left middle rect
			r.top = Internal_Widget.AbsolutePosition.top + t.BorderYSize;
			r.width =t.BorderXSize;
			r.height = Internal_Widget.AbsolutePosition.height - 2*t.BorderYSize;
		
			Internal::WidgetRenderer->DrawTexturedRect_NoClip(tex, t.Left_Middle, r, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
			// right middle rect
			r.left = Internal_Widget.AbsolutePosition.left + Internal_Widget.AbsolutePosition.width - t.BorderXSize;
		
			Internal::WidgetRenderer->DrawTexturedRect_NoClip(tex, t.Right_Middle, r, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
			//middle rect
			r.width = Internal_Widget.AbsolutePosition.width - 2*t.BorderXSize;
			r.left = Internal_Widget.AbsolutePosition.left + t.BorderXSize;
		
			Internal::WidgetRenderer->DrawTexturedRect_NoClip(tex, t.Middle, r, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
		
		} else {
			Utilities::UVs temp;
			CopyUvs(temp, Internal_Image.Textures[Internal_Image.ImageShown].Texture);
			Internal::WidgetRenderer->DrawTexturedRect_NoClip(Internal_Image.Textures[Internal_Image.ImageShown].Texture, temp, Internal_Widget.AbsolutePosition, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
	
		}
	}
}
void MY_UI::Controls::Image::Draw_Clip(){
	if(Internal_Image.ImageShown != -1){// only draw an image if there is an image to be shown
		if(Internal_Widget.Hidden) return;
		
		Utilities::Rect r =Internal_Widget.AbsolutePosition;
		MY_UI::Utilities::Texture& tex = Internal_Image.Textures[Internal_Image.ImageShown].Texture;
		auto& t = Internal_Image.Textures[Internal_Image.ImageShown];
		if(t.DrawBorder){
			// top rect
			r.height =t.BorderYSize;
			Internal::WidgetRenderer->DrawTexturedRect_Clip(tex, t.Top, r, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
			// bottom rect
			r.top = Internal_Widget.AbsolutePosition.top + Internal_Widget.AbsolutePosition.height -t.BorderYSize;
	
			Internal::WidgetRenderer->DrawTexturedRect_Clip(tex, t.Bottom, r, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
			// left middle rect
			r.top = Internal_Widget.AbsolutePosition.top + t.BorderYSize;
			r.width =t.BorderXSize;
			r.height = Internal_Widget.AbsolutePosition.height - 2*t.BorderYSize;
		
			Internal::WidgetRenderer->DrawTexturedRect_Clip(tex, t.Left_Middle, r, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
			// right middle rect
			r.left = Internal_Widget.AbsolutePosition.left + Internal_Widget.AbsolutePosition.width - t.BorderXSize;
		
			Internal::WidgetRenderer->DrawTexturedRect_Clip(tex, t.Right_Middle, r, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
			//middle rect
			r.width = Internal_Widget.AbsolutePosition.width - 2*t.BorderXSize;
			r.left = Internal_Widget.AbsolutePosition.left + t.BorderXSize;
		
			Internal::WidgetRenderer->DrawTexturedRect_Clip(tex, t.Middle, r, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
		
		} else {
			Utilities::UVs temp;
			CopyUvs(temp, Internal_Image.Textures[Internal_Image.ImageShown].Texture);
			Internal::WidgetRenderer->DrawTexturedRect_Clip(Internal_Image.Textures[Internal_Image.ImageShown].Texture, temp, Internal_Widget.AbsolutePosition, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
	
		}
	}
}
bool MY_UI::Controls::Image::Add_Texture(std::string filename, int slot){
	MY_UI::Internal::cImage_Helper h;
	h.OwnsTexture = true;// this class is loading the texture, so we own it!!
	h.Texture.FileName = filename;
	// if slot is -1, do a standard push back
	Utilities::Texture* tex=0;
	if(slot == -1 || slot == Internal_Image.Textures.size()) {
		Internal_Image.Textures.push_back(h);
		tex = &Internal_Image.Textures.back().Texture;// get the address of the texture
	}else {// insert somewhere else in the array, check it out
		if(slot > static_cast<int>(Internal_Image.Textures.size())){
			Internal_Image.Textures.resize(slot +1);// slot is an array, so need to adjust by one
		} 
		if(Internal_Image.Textures[slot].Texture.Tex != 0){// a texture is already in this slot, try to free it if we own it
			if(Internal_Image.Textures[slot].OwnsTexture){// this class owns the texture, release it
				MY_UI::Internal::WidgetRenderer->FreeTexture(&Internal_Image.Textures[slot].Texture);
			} else {// this class doesnt own the texture, just clear it
				Internal_Image.Textures[slot].Texture.clear();
			}
		}// otherwise, the texture is empty 
		tex = &Internal_Image.Textures[slot].Texture;// get the address of the texture
	}
	if(!MY_UI::Internal::WidgetRenderer->LoadTexture(tex)){
		// bad load, remove the texture from our list and return false
		if(slot ==-1) Internal_Image.Textures.pop_back();
		else Internal_Image.Textures[slot].Texture.clear();
		return false;
	}
	return true;// good load
}
void MY_UI::Controls::Image::Add_Texture(Utilities::Texture& te, bool takeownership, int slot){
	MY_UI::Internal::cImage_Helper h;
	h.OwnsTexture = takeownership;
	h.Texture= te;
	if(slot == -1 || slot == Internal_Image.Textures.size()) {
		Internal_Image.Textures.push_back(h);
	}else {// insert somewhere else in the array, check it out
		if(slot > static_cast<int>(Internal_Image.Textures.size())){
			Internal_Image.Textures.resize(slot +1);// slot is an array, so need to adjust by one
		} 
		if(Internal_Image.Textures[slot].Texture.Tex != 0){// a texture is already in this slot, try to free it if we own it
			if(Internal_Image.Textures[slot].OwnsTexture){// this class owns the texture, release it
				MY_UI::Internal::WidgetRenderer->FreeTexture(&Internal_Image.Textures[slot].Texture);
			} else {// this class doesnt own the texture, just clear it
				Internal_Image.Textures[slot].Texture.clear();
			}
		}// otherwise, the texture is empty 
		Internal_Image.Textures[slot].Texture = te;
		Internal_Image.Textures[slot].OwnsTexture = takeownership;
	}

}
void MY_UI::Controls::Image::SetUVs(const float u1, const float v1, const float u2, const float v2, unsigned int index){
	Internal_Image.Textures[index].Texture.u1 = u1;
	Internal_Image.Textures[index].Texture.v1 = v1;
	Internal_Image.Textures[index].Texture.u2 = u2;
	Internal_Image.Textures[index].Texture.u2 = u2;
	if(Internal_Image.Textures[index].DrawBorder){//recalculate the border uvs
		SetImageBorderSize(Internal_Image.Textures[index].BorderXSize, Internal_Image.Textures[index].BorderYSize);
	}
}
bool MY_UI::Controls::Image::ImageExists(unsigned int index){
	if(index < Internal_Image.Textures.size()){
		return Internal_Image.Textures[index].Texture.Tex != 0;
	}
	return false;
}
void MY_UI::Controls::Image::Set_ShownImage(unsigned int index){
	Internal::RootWidget->WidgetHasChanged=true;
	Internal_Image.ImageShown = index;
}
unsigned int MY_UI::Controls::Image::Get_ShownImage(){
	return Internal_Image.ImageShown;
}
void MY_UI::Controls::Image::ClearImages(){
	for( auto i(Internal_Image.Textures.begin()); i != Internal_Image.Textures.end(); i++){
		if((*i).OwnsTexture){
			MY_UI::Internal::WidgetRenderer->FreeTexture(&((*i).Texture));
		}
	}
	Internal_Image.Textures.clear();
	Internal_Image.ImageShown=-1;
}
void MY_UI::Controls::Image::ClearImage(int index){
	if(index >= static_cast<int>(Internal_Image.Textures.size())) return;// buffer overrun
	if((Internal_Image.Textures[index]).OwnsTexture){
		MY_UI::Internal::WidgetRenderer->FreeTexture(&Internal_Image.Textures[index].Texture);
	}
	Internal_Image.Textures[index].Texture.clear();
	if(index == Internal_Image.ImageShown){// removing the shown image.. try to find an image to show
		if(ImageExists(0)){
			Set_ShownImage(0);
		} else Set_ShownImage(-1);
	}
	
}
void MY_UI::Controls::Image::Set_Texture(Utilities::Texture& tex, bool takeownership){
	ClearImages();
	Add_Texture(tex, takeownership);
	
	Set_ShownImage(0);// set this  so the object can draw
}
bool MY_UI::Controls::Image::Set_Texture(std::string filename){
	ClearImages();
	if(Add_Texture(filename)){
		Set_ShownImage(0);// set this  so the object can draw
		return true;
	}
	return false;
}
void MY_UI::Controls::Image::SetBorderDrawn(bool drawnborder, unsigned int index){
	unsigned int beg =0;
	unsigned int end = static_cast<unsigned int>(Internal_Image.Textures.size());
	if(index != 9999) {
		beg = index;
		end = index  +1;
	}
	for(; beg< end; beg++){
		Internal_Image.Textures[beg].DrawBorder = drawnborder; 
	}
}
void MY_UI::Controls::Image::SetImageBorderSize(int x, int y, bool drawnborder, unsigned int index){
		unsigned int beg =0;
		unsigned int end = static_cast<unsigned int>(Internal_Image.Textures.size());
		if(index != 9999) {
			beg = index;
			end = index  +1;
		}
		for(; beg< end; beg++){
			Internal_Image.Textures[beg].DrawBorder = drawnborder; 
			MY_UI::Utilities::Texture& tex = Internal_Image.Textures[beg].Texture;
			float tu1(tex.u1), tv1(tex.v1), tv2(tex.v2), tu2(tex.u2);
			auto& t = Internal_Image.Textures[beg];
			t.BorderXSize = x;
			t.BorderYSize = y;
			CopyUvs(t.Top, tex);
			CopyUvs(t.Bottom, tex);
			CopyUvs(t.Left_Middle, tex);
			CopyUvs(t.Right_Middle, tex);
			CopyUvs(t.Middle, tex);
		
			float ydiv = static_cast<float>(y)/SKIN_SIZE;
			float xdiv = static_cast<float>(x)/SKIN_SIZE;
		
			// top rect
			t.Top.v2 = tv1 + ydiv;
	
			// bottom rect
			t.Bottom.v2 = tv2;
			t.Bottom.v1 = tv2 - ydiv;
	
			// left middle rect
			t.Left_Middle.u1 = tu1;
			t.Left_Middle.v1 = ydiv + tv1;
			t.Left_Middle.v2 = tv2 - ydiv;
			t.Left_Middle.u2 = xdiv + tu1;

			// right middle rect
			t.Right_Middle.u1 = tu2 - xdiv;
			t.Right_Middle.u2 = tu2;
			t.Right_Middle.v1 = ydiv + tv1;
			t.Right_Middle.v2 = tv2 - ydiv;

			//middle rect

			t.Middle.u1 = xdiv + tu1;
			t.Middle.v1 = tv1 + ydiv;
			t.Middle.u2 = tu2 - xdiv;
			t.Middle.v2 = tv2 - ydiv;
		}
		Internal::RootWidget->WidgetHasChanged = true;
}


///////////////////////////////////////////////////////////  ColoredRect


void MY_UI::Controls::ColoredRect::Draw_NoClip(){// this will draw the widget immediately. Sometimes, you need to draw stuff right now!
	if(Internal_Widget.Hidden) return;	
	auto skin = MY_UI::Internal::WidgetSkin;
	Utilities::UVs tempuvs;
	Utilities::CopyUvs(tempuvs, skin->White_Box);
	Internal::WidgetRenderer->DrawTexturedRect_NoClip(skin->White_Box, tempuvs, Internal_Widget.AbsolutePosition, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
}

void MY_UI::Controls::ColoredRect::Draw_Clip(){
	if(Internal_Widget.Hidden) return;	
	auto skin = MY_UI::Internal::WidgetSkin;
	Utilities::UVs tempuvs;
	Utilities::CopyUvs(tempuvs, skin->White_Box);
	Internal::WidgetRenderer->DrawTexturedRect_Clip(skin->White_Box, tempuvs, Internal_Widget.AbsolutePosition, Internal_Widget.Color_Top_Left, Internal_Widget.Color_Top_Right, Internal_Widget.Color_Bottom_Left, Internal_Widget.Color_Bottom_Right);
}


