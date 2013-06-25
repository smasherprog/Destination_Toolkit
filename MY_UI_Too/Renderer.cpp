#include "PCH.h"
#include "Renderer.h"
#include <math.h>
#include <vector>
#include <fstream>
#include "IFont.h"
#include "Common.h"

void MY_UI_Too::Renderer::Begin(){
	Get_Render_States();// make a copy of the render states
	DrawCalls =0;
}
void MY_UI_Too::Renderer::End(){
	Restore_Render_States();// restore the settings
}

void MY_UI_Too::Renderer::Draw(){

	for(size_t i(0); i < Draw_State_Index +1; i++){
		if(Draw_States[i].NumVerts == 0) continue;
		Draw(Draw_States[i]);
		++DrawCalls;
		Draw_States[i].changed=false;
	}

}
void MY_UI_Too::Renderer::StartNewBatch() { 
	Draw_State_Index=0; 
	for(size_t i(0); i < Draw_States.size(); i++) Draw_States[i].NumVerts=0;  
}
void MY_UI_Too::Renderer::GotoNextBufferSlot(){		
	if(Draw_States.size() <= Draw_State_Index +1)  Draw_States.resize(Draw_States.size() + 16);// add more space if needed
	Draw_States[ Draw_State_Index ].Verts.resize(Draw_States[ Draw_State_Index ].NumVerts);// resize the buffer in case there is hardly any data inside
	Draw_State_Index+=1;// goto the next index
}
void MY_UI_Too::Renderer::AddVert( float x, float y, float u, float v, Utilities::Color col){
	if(Draw_States[ Draw_State_Index ].Verts.size() <= 4 + Draw_States[ Draw_State_Index ].NumVerts ) Draw_States[ Draw_State_Index ].Verts.resize(Draw_States[ Draw_State_Index ].Verts.size() + 200);
	if ( Draw_States[ Draw_State_Index ].NumVerts >= VERT_BUFFER_SIZE) GotoNextBufferSlot();

	MyDrawState& drawstate = Draw_States[ Draw_State_Index ];
	VERTEXFORMAT2D& vert = drawstate.Verts[drawstate.NumVerts];
	vert.x = (x*Inv_Wndx) -1.0f;
	vert.y = (y*Inv_Wndy) +1.0f;
	vert.u = u;
	vert.v = v;	
	vert.color = col.color;
	drawstate.NumVerts+=1;
	drawstate.changed=true;
}
bool MY_UI_Too::Renderer::SetTexture(const MY_UI_Too::Interfaces::ITexture* pTexture, bool drawingnow){
	void* pImage = pTexture->Get_Texture();
	if ( pImage ==nullptr) return false;// Missing image, not loaded properly?
	if( (CurrentTexture!= pImage) | (drawingnow)){
		GotoNextBufferSlot();
	}
	Draw_States[ Draw_State_Index ].texture = pImage;
	CurrentTexture =pImage;
	return true;
}

void MY_UI_Too::Renderer::DrawTexturedRect_Clip(const MY_UI_Too::Interfaces::ITexture* pTexture,const  MY_UI_Too::Utilities::UVs& uvs, const MY_UI_Too::Utilities::Rect rect,  const MY_UI_Too::Utilities::Color color_tl, const MY_UI_Too::Utilities::Color color_tr, const MY_UI_Too::Utilities::Color color_bl, const MY_UI_Too::Utilities::Color color_br, bool drawnow){
	if(!SetTexture(pTexture, drawnow)) return;
	assert(!ClipRects.empty());
	Utilities::Point tl(rect.left, rect.top);
	Utilities::Point tr(rect.left + rect.width, rect.top);
	Utilities::Point bl(rect.left, rect.top + rect.height);
	Utilities::Point br(rect.left + rect.width, rect.top + rect.height);
	// if all the points are not within the cliprect, dont draw it
	bool brin = ClipRects.back().Intersect(br);
	bool trin = ClipRects.back().Intersect(tr);

	bool blin = ClipRects.back().Intersect(bl);
	bool tlin = ClipRects.back().Intersect(tl);

	if( (!brin) & (!trin) & (!blin) & (!tlin)) return;// all points are outside the cliprect


	float left= static_cast<float>(rect.left);
	float top = static_cast<float>(rect.top);
	float width = static_cast<float>(rect.width);
	float right = width + left;
	float height = static_cast<float>(rect.height);
	float bottom = height + top;
	// resize the buffer if needed
	if(Draw_States[ Draw_State_Index ].Verts.size() <= 4 + Draw_States[ Draw_State_Index ].NumVerts ) Draw_States[ Draw_State_Index ].Verts.resize(Draw_States[ Draw_State_Index ].Verts.size() + 200);
	if( (brin) & (trin) & (blin) & (tlin)){// all points are fully contained inside the cliprect

		AddVert( left, top,					uvs.u1, uvs.v1, color_tl );
		AddVert( right, top,				uvs.u2, uvs.v1, color_tr );
		AddVert( left, bottom,				uvs.u1, uvs.v2, color_bl );
		AddVert( right, bottom,				uvs.u2, uvs.v2, color_br );


	} else {// this means the rect is partially in the clip region. Use the cpu to clip it

		Utilities::Rect& r = ClipRects.back();
		float newleft= static_cast<float>(Utilities::Clamp<int>(rect.left, r.left, r.left + r.width));
		float newtop = static_cast<float>(Utilities::Clamp<int>(rect.top, r.top, r.top + r.height));
		float newright = static_cast<float>(Utilities::Clamp<int>(rect.width + rect.left, r.left, r.left + r.width));
		float newbottom = static_cast<float>(Utilities::Clamp<int>(rect.height + rect.top, r.top, r.top + r.height));

		float difleft = newleft - left;
		float diftop = newtop - top;
		float difright = newright - right;
		float difbottom = newbottom - bottom;

		difleft /= width;
		diftop /= height;
		difright /= width;
		difbottom /= height;

		float u1 = uvs.u1;
		float v1 = uvs.v1;
		float u2 = uvs.u2;
		float v2 = uvs.v2;

		float uwidth = u2 - u1;
		float vheight = v2 - v1;

		u1 = u1 + (uwidth*difleft);
		u2 = u2 + (uwidth*difright);
		v1 = v1 + (vheight*diftop);
		v2 = v2 + (vheight*difbottom);

		AddVert( newleft, newtop,					u1, v1, color_tl );
		AddVert( newright, newtop,					u2, v1, color_tr );
		AddVert( newleft, newbottom,				u1, v2, color_bl );
		AddVert( newright, newbottom,				u2, v2, color_br );

	}
}

void MY_UI_Too::Renderer::DrawTexturedRect_NoClip(const MY_UI_Too::Interfaces::ITexture* pTexture, const MY_UI_Too::Utilities::UVs& uvs, const MY_UI_Too::Utilities::Rect rect, const MY_UI_Too::Utilities::Color color_tl, const MY_UI_Too::Utilities::Color color_tr, const MY_UI_Too::Utilities::Color color_bl, const MY_UI_Too::Utilities::Color color_br, bool drawnow){
	if(!SetTexture(pTexture, drawnow)) return;

	float left= static_cast<float>(rect.left);
	float top = static_cast<float>(rect.top);
	float width = static_cast<float>(rect.width);
	float right = width + left;
	float height = static_cast<float>(rect.height);
	float bottom = height + top;
	if(Draw_States[ Draw_State_Index ].Verts.size() <= 4 + Draw_States[ Draw_State_Index ].NumVerts ) Draw_States[ Draw_State_Index ].Verts.resize(Draw_States[ Draw_State_Index ].Verts.size() + 200);
	AddVert( left, top,					uvs.u1, uvs.v1, color_tl );
	AddVert( right, top,				uvs.u2, uvs.v1, color_tr );
	AddVert( left, bottom,				uvs.u1, uvs.v2, color_bl );
	AddVert( right, bottom,				uvs.u2, uvs.v2, color_br );
}
void MY_UI_Too::Renderer::DrawText_NoClip(const MY_UI_Too::Interfaces::ITexture* pTexture, const MY_UI_Too::Interfaces::IFont* font, const std::string text, const MY_UI_Too::Utilities::Point startinpos, const unsigned int fontsize, const MY_UI_Too::Utilities::Color color_tl,const  MY_UI_Too::Utilities::Color color_tr, const MY_UI_Too::Utilities::Color color_bl, const MY_UI_Too::Utilities::Color color_br, bool drawnow){
	if(!SetTexture(pTexture, drawnow)) return;
	int startleft= startinpos.x;
	float skinwidth = (float)pTexture->Get_Dimensions().left;
	float skinheight = (float)pTexture->Get_Dimensions().top;
	float fontsizef = (float)fontsize;
	float fontscale = fontsizef/(float)FONT_CREATE_SIZE;

	Utilities::UVs spaceuvs = font->Get_Char('a');
	unsigned int spacewidth=(int)(skinwidth*(spaceuvs.u2 - spaceuvs.u1)*fontscale);
	spaceuvs = font->Get_Char(')');
	unsigned int vertheight =(int)(NEXTLINEPERCENT*skinheight*(spaceuvs.v2 - spaceuvs.v1) + skinheight*(spaceuvs.v2 - spaceuvs.v1));
	MY_UI_Too::Utilities::Point startin_pos=startinpos;

	//goto http://www.asciitable.com/ for ascii info
	for(auto beg = text.begin(); beg!= text.end(); beg++){

		char c = *beg;
		if(c == 0)continue;
		else if(c==9) startin_pos.left+=(spacewidth*8);// a tab is 8 spaces
		else if(c==32) startin_pos.left+=spacewidth;
		else if(c==13){
			if(beg+1 != text.end()){
				if(*(beg+1) == 10) {
					beg++;//advance a character.. some people still think \r\n is appropriate for a newline instead of just \n
				}
			}
			startin_pos.top+=vertheight;
			startin_pos.left = startleft;
		} else if(c==10) {
			startin_pos.top+=vertheight;
			startin_pos.left = startleft;
		}
		else {
			if( (c<START_CHAR) | (c> END_CHAR)) {
				OUTPUT_DEBUG_MSG("Measure string encountered a non keyboard character. The character is(dec): "<<(int)c<<"  This character is being ignored");
				continue;
			}
			Utilities::UVs uvs = font->Get_Char(c);
			float width = skinwidth*(uvs.u2 - uvs.u1)*fontscale;

			float left= static_cast<float>(startin_pos.x);
			float top = static_cast<float>(startin_pos.y);
			float right = width + left;
			float height = fontsizef;
			float bottom = height + top;

			AddVert( left, top,					uvs.u1, uvs.v1, color_tl );
			AddVert( right, top,				uvs.u2, uvs.v1, color_tr );
			AddVert( left, bottom,				uvs.u1, uvs.v2, color_bl );
			AddVert( right, bottom,				uvs.u2, uvs.v2, color_br );
			startin_pos.x+=(int)width+1;// add at least one pixel
		}
	}
}
void MY_UI_Too::Renderer::DrawTextCaret_NoClip(const MY_UI_Too::Interfaces::ITexture* pTexture, const MY_UI_Too::Interfaces::IFont* font, const std::string text, const MY_UI_Too::Utilities::Point startinpos, const unsigned int fontsize, unsigned int careindex, const MY_UI_Too::Utilities::Color color_tl,const  MY_UI_Too::Utilities::Color color_tr, const MY_UI_Too::Utilities::Color color_bl, const MY_UI_Too::Utilities::Color color_br, bool drawnow){
	if(!SetTexture(pTexture, drawnow)) return;
	int startleft= startinpos.x;
	float skinwidth = (float)pTexture->Get_Dimensions().left;
	float skinheight = (float)pTexture->Get_Dimensions().top;
	float fontsizef = (float)fontsize;
	float fontscale = fontsizef/(float)FONT_CREATE_SIZE;

	Utilities::UVs spaceuvs = font->Get_Char('a');
	unsigned int spacewidth=(int)(skinwidth*(spaceuvs.u2 - spaceuvs.u1)*fontscale);
	spaceuvs = font->Get_Char(')');
	unsigned int vertheight =(int)(NEXTLINEPERCENT*skinheight*(spaceuvs.v2 - spaceuvs.v1) + skinheight*(spaceuvs.v2 - spaceuvs.v1));
	MY_UI_Too::Utilities::Point startin_pos=startinpos;
	unsigned int index=0;
	//goto http://www.asciitable.com/ for ascii info
	for(auto beg = text.begin(); beg!= text.end(); beg++){

		char c = *beg;
		if(c == 0)continue;
		else if(c==9) startin_pos.left+=(spacewidth*8);// a tab is 8 spaces
		else if(c==32) startin_pos.left+=spacewidth;
		else if(c==13){
			if(beg+1 != text.end()){
				if(*(beg+1) == 10) {
					beg++;//advance a character.. some people still think \r\n is appropriate for a newline instead of just \n
				}
			}
			startin_pos.top+=vertheight;
			startin_pos.left = startleft;
		} else if(c==10) {
			startin_pos.top+=vertheight;
			startin_pos.left = startleft;
		}
		else {
			if( (c<START_CHAR) | (c> END_CHAR)) {
				OUTPUT_DEBUG_MSG("Measure string encountered a non keyboard character. The character is(dec): "<<(int)c<<"  This character is being ignored");
				continue;
			}
			Utilities::UVs uvs = font->Get_Char(c);
			float width = skinwidth*(uvs.u2 - uvs.u1)*fontscale;
			if(index == careindex){
				uvs = font->Get_Char('|');
				width = skinwidth*(uvs.u2 - uvs.u1)*fontscale;
				float left= static_cast<float>(startin_pos.x);
				float top = static_cast<float>(startin_pos.y);
				float right = width + left;
				float height = fontsizef;
				float bottom = height + top;

				AddVert( left, top,					uvs.u1, uvs.v1, color_tl );
				AddVert( right, top,				uvs.u2, uvs.v1, color_tr );
				AddVert( left, bottom,				uvs.u1, uvs.v2, color_bl );
				AddVert( right, bottom,				uvs.u2, uvs.v2, color_br );
				return;
			}
			startin_pos.x+=(int)width+1;// add at least one pixel
		}
		index+=1;
	}
	Utilities::UVs uvs = font->Get_Char('|');
	float width = skinwidth*(uvs.u2 - uvs.u1)*fontscale;
	float left= static_cast<float>(startin_pos.x);
	float top = static_cast<float>(startin_pos.y);
	float right = width + left;
	float height = fontsizef;
	float bottom = height + top;

	AddVert( left, top,					uvs.u1, uvs.v1, color_tl );
	AddVert( right, top,				uvs.u2, uvs.v1, color_tr );
	AddVert( left, bottom,				uvs.u1, uvs.v2, color_bl );
	AddVert( right, bottom,				uvs.u2, uvs.v2, color_br );
}
MY_UI_Too::Utilities::Point MY_UI_Too::Renderer::Measure_String(const MY_UI_Too::Utilities::Point skinsize,const  MY_UI_Too::Interfaces::IFont* font, const unsigned int fontsize, const std::string text){
	MY_UI_Too::Utilities::Point dims;
	float skinwidth = (float)skinsize.x;
	float skinheight = (float)skinsize.y;
	float fontscale = (float)fontsize/(float)FONT_CREATE_SIZE;
	Utilities::UVs spaceuvs = font->Get_Char('a');
	unsigned int spacewidth=(int)(skinwidth*(spaceuvs.u2 - spaceuvs.u1)*fontscale);
	spaceuvs = font->Get_Char(')');
	unsigned int vertheight =(int)(NEXTLINEPERCENT*skinheight*(spaceuvs.v2 - spaceuvs.v1)*fontscale + skinheight*(spaceuvs.v2 - spaceuvs.v1)*fontscale);

	//goto http://www.asciitable.com/ for ascii info
	for(auto beg = text.begin(); beg!= text.end(); beg++){

		char c = *beg;
		if(c == 0)continue;
		else if(c==9) dims.left+=(spacewidth*8);// a tab is 8 spaces
		else if(c==32) dims.left+=spacewidth;
		else if(c==13){
			if(beg+1 != text.end()){
				if(*(beg+1) == 10) {
					beg++;//advance a character.. some people still think \r\n is appropriate for a newline instead of just \n
				}
			}
			dims.top+=vertheight;
		} else if(c==10) dims.top+=vertheight;
		else {
			if((c<START_CHAR) | (c> END_CHAR)) {
				OUTPUT_DEBUG_MSG("Measure string encountered a non keyboard character. The character is(dec): "<<(int)c<<"  This character is being ignored");
				continue;
			}
			Utilities::UVs uvs = font->Get_Char(c);
			dims.left += (int)(skinwidth*(uvs.u2 - uvs.u1)*fontscale) +1;// add one pixel
		}
	}
	return dims;
}
