#ifndef MY_UI_RENDERER_H
#define MY_UI_RENDERER_H
#include "IRenderer.h"
#include "../Utilities/Utilities.h"
#include <vector>

/*
This renderer defines most of the functionality. There are just a couple of functions that need to be written in a derived class


		virtual bool Init() override;
		virtual void DeInit() override;
	
		virtual MY_UI_Too::Interfaces::ITexture* LoadTexture(std::string filename, bool as_rendertarget=false) override;
		virtual MY_UI_Too::Interfaces::ITexture* CreateTexture(unsigned int width, unsigned int height, Utilities::Color* buffer=nullptr, bool as_rendertarget=false) override;
		virtual void Set_Render_Target(MY_UI_Too::Interfaces::ITexture* texture)override;

	protected:
		
		virtual void Get_Render_States()override;
		virtual void Restore_Render_States()override;

		virtual void Draw(MyDrawState& drawstate) override;

*/

namespace MY_UI_Too{
	class Renderer : public MY_UI_Too::Interfaces::IRenderer
	{
	public:

		Renderer() :Draw_State_Index(0), Reset_RenderTarget(false) {}
		virtual ~Renderer() {}

		virtual bool Init() override{ Draw_States.resize(1); Reset_RenderTarget= false; return true;}	// initialize with one buffer slot
		virtual void DeInit() =0;

		virtual void Begin()override;
		virtual void End()override;
		// start new batch should be called if the previous contents of all the buffered data are no longer valid and the entire buffer should be rebuilt
		virtual void StartNewBatch() override;
		virtual void Draw() override;// this will draw everything that has been buffered so far

		// the four draw functions below only buffer the data, they do not actually draw anything until DrawBuffred is called
		virtual void DrawTexturedRect_Clip( MY_UI_Too::Interfaces::ITexture* Texture,  MY_UI_Too::Utilities::UVs& uvs, MY_UI_Too::Utilities::Rect rect, MY_UI_Too::Utilities::Color color_tl = MY_UI_Too::Utilities::White, MY_UI_Too::Utilities::Color color_tr = MY_UI_Too::Utilities::White, MY_UI_Too::Utilities::Color color_bl = MY_UI_Too::Utilities::White, MY_UI_Too::Utilities::Color color_br = MY_UI_Too::Utilities::White, bool getnewbuffer = false) override;
		virtual void DrawTexturedRect_NoClip( MY_UI_Too::Interfaces::ITexture* Texture,  MY_UI_Too::Utilities::UVs& uvs, MY_UI_Too::Utilities::Rect rect, MY_UI_Too::Utilities::Color color_tl = MY_UI_Too::Utilities::White, MY_UI_Too::Utilities::Color color_tr = MY_UI_Too::Utilities::White, MY_UI_Too::Utilities::Color color_bl = MY_UI_Too::Utilities::White, MY_UI_Too::Utilities::Color color_br = MY_UI_Too::Utilities::White, bool getnewbuffer = false) override;

		virtual void DrawText_NoClip(MY_UI_Too::Interfaces::ITexture* pTexture, MY_UI_Too::Interfaces::IFont* font, std::string txt, MY_UI_Too::Utilities::Point startinpos, unsigned int fontsize, MY_UI_Too::Utilities::Color color_tl = MY_UI_Too::Utilities::White, MY_UI_Too::Utilities::Color color_tr = MY_UI_Too::Utilities::White, MY_UI_Too::Utilities::Color color_bl = MY_UI_Too::Utilities::White, MY_UI_Too::Utilities::Color color_br = MY_UI_Too::Utilities::White, bool getnewbuffer=false) override;
		virtual MY_UI_Too::Utilities::Point Measure_String(MY_UI_Too::Utilities::Point skinsize, MY_UI_Too::Interfaces::IFont* font, unsigned int fontsize, std::string text);

		virtual void StartClip(MY_UI_Too::Utilities::Rect& rect) override{ ClipRects.push_back(rect);}
		virtual void EndClip() override{ClipRects.pop_back();}

		virtual void Set_Size(MY_UI_Too::Utilities::Point screensize) override { Screen_Dimension = screensize; Inv_Wndx = (1.0f/static_cast<float>(screensize.x))*2.0f; Inv_Wndy = (1.0f/static_cast<float>(screensize.y))*-2.0f; }
		virtual MY_UI_Too::Utilities::Point Get_Size()override { return Screen_Dimension; }

		virtual unsigned int GetDrawCalls()const override{ return DrawCalls;}

	protected:

		virtual void Draw(MyDrawState& drawstate)=0;
		virtual bool SetTexture(MY_UI_Too::Interfaces::ITexture* pTexture, bool getnewbuffer)override;
		virtual void AddVert( float x, float y, float u, float v, Utilities::Color col)override;
		virtual void GotoNextBufferSlot()override;

		std::vector<MY_UI_Too::Utilities::Rect> ClipRects;
		float				Inv_Wndx, Inv_Wndy;// screen size in pixels

		std::vector<MyDrawState>  Draw_States;

		unsigned int				Draw_State_Index, DrawCalls;
		Utilities::Point			Screen_Dimension;
		void*						CurrentTexture;
		bool Reset_RenderTarget;
	};

};
#endif
