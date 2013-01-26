#ifndef CWIDGETRENDERER_H
#define CWIDGETRENDERER_H
#include <string>
#include "assert.h"
#include "cWidgetUtilities.h"
#include "cWidgetInternals.h"

namespace MY_UI{
	// this renderer is a Buffered renderer, meaning all data sent to is is cached so work does not need to be redone each fram if it is not needed. This greatly improves rendering speed
	class cWidgetRenderer{// This is a purly virtual base class that users should derive from in order to create an Open Gl renderer, or a Direct X renderer
	public:

		cWidgetRenderer() : DrawCalls(0) {  }
		virtual ~cWidgetRenderer(){}// make sure to free any resorces that you allocate in the derived destructor 
		virtual bool Init()=0;
		virtual void DeInit() =0;
		virtual void Begin()=0;// this should set all necessary render states, i.e. alpha blending, no depth testing, etc
		virtual void End()=0;// this should restore all previous device states
		// all draw functions should buffer what is sent to them and only actually draw the stuff when Flush is called. 
		// This should greatly improve drawing speed and allow for batching
		virtual void StartNewBatch()=0;
		virtual void DrawBufferd()=0;
		virtual void DrawBack(bool popback)=0;

		// these functions will buffer everything sent to them and nothing will be drawn until the drawbuffered is called
		virtual void DrawTexturedRect_Clip( MY_UI::Utilities::Texture& Texture, MY_UI::Utilities::UVs& uvs, MY_UI::Utilities::Rect rect, MY_UI::Utilities::Color color_tl = MY_UI::Utilities::White, MY_UI::Utilities::Color color_tr = MY_UI::Utilities::White, MY_UI::Utilities::Color color_bl = MY_UI::Utilities::White, MY_UI::Utilities::Color color_br = MY_UI::Utilities::White, bool getnewbuffer=false)=0;
		virtual void DrawTexturedRect_NoClip( MY_UI::Utilities::Texture& Texture,  MY_UI::Utilities::UVs& uvs, MY_UI::Utilities::Rect rect, MY_UI::Utilities::Color color_tl = MY_UI::Utilities::White, MY_UI::Utilities::Color color_tr = MY_UI::Utilities::White, MY_UI::Utilities::Color color_bl = MY_UI::Utilities::White, MY_UI::Utilities::Color color_br = MY_UI::Utilities::White, bool getnewbuffer=false)=0;
		// user must pass a white texture to be used for the line drawing . . . .
		virtual void DrawLine_Clip( MY_UI::Utilities::Texture& pTexture, MY_UI::Utilities::Point beg, MY_UI::Utilities::Point end, int thickness = 4, MY_UI::Utilities::Color color_beg = MY_UI::Utilities::White, MY_UI::Utilities::Color color_end = MY_UI::Utilities::White, bool getnewbuffer=false)=0;
		virtual void DrawLine_NoClip(MY_UI::Utilities::Texture& pTexture, MY_UI::Utilities::Point beg, MY_UI::Utilities::Point end, int thickness = 4, MY_UI::Utilities::Color color_beg = MY_UI::Utilities::White, MY_UI::Utilities::Color color_end = MY_UI::Utilities::White , bool getnewbuffer=false)=0;

		virtual void StartClip(MY_UI::Utilities::Rect& rect)=0;
		virtual void EndClip()=0;

		virtual bool LoadTexture( MY_UI::Utilities::Texture* Texture)=0;
		virtual void FreeTexture( MY_UI::Utilities::Texture* Texture)=0;
		
		virtual void OnResize(unsigned int x, unsigned int y) =0;// this is called when the screen sets a new window dimensions
	
		unsigned int GetDrawCalls(){ return DrawCalls; }
		unsigned int SetDrawCalls(unsigned int x){ return DrawCalls=x; }

		unsigned int DrawCalls;
	};

};

#endif
