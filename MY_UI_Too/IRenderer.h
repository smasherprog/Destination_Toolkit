#ifndef CWIDGETRENDERER_H
#define CWIDGETRENDERER_H
#include <string>
#include "ITexture.h"
#include <vector>

struct VERTEXFORMAT2D
{
	float x, y;
	unsigned int color;
	float u, v;
};
#define VERT_BUFFER_SIZE 32768

class MyDrawState{
public:
	MyDrawState() : Verts(VERT_BUFFER_SIZE), texture(0), NumVerts(0), changed(true) {}
	std::vector<VERTEXFORMAT2D>	Verts;
	void* texture;
	unsigned int NumVerts;
	bool changed;
};

namespace MY_UI_Too{
	namespace Interfaces{
		class IFont;
		// this renderer is a Buffered renderer, meaning all data sent to is is cached so work does not need to be redone each fram if it is not needed. This greatly improves rendering speed
		class IRenderer{// This is a purly virtual base class that users should derive from in order to create an Open Gl renderer, or a Direct X renderer
		public:

			virtual ~IRenderer(){ }// make sure to free any resorces that you allocate in the derived destructor 
			virtual bool Init()=0;
			virtual void DeInit() =0;

			virtual void Begin()=0;
			virtual void End()=0;

			virtual void StartNewBatch()=0;
			virtual void Draw()=0;

			virtual void DrawTexturedRect_Clip(const MY_UI_Too::Interfaces::ITexture* Texture, const MY_UI_Too::Utilities::UVs& uvs, const MY_UI_Too::Utilities::Rect rect, const MY_UI_Too::Utilities::Color color_tl = MY_UI_Too::Utilities::White, const MY_UI_Too::Utilities::Color color_tr = MY_UI_Too::Utilities::White, const MY_UI_Too::Utilities::Color color_bl = MY_UI_Too::Utilities::White, const MY_UI_Too::Utilities::Color color_br = MY_UI_Too::Utilities::White, bool getnewbuffer=false)=0;
			virtual void DrawTexturedRect_NoClip( const MY_UI_Too::Interfaces::ITexture* Texture, const MY_UI_Too::Utilities::UVs& uvs, const MY_UI_Too::Utilities::Rect rect, const MY_UI_Too::Utilities::Color color_tl = MY_UI_Too::Utilities::White,const  MY_UI_Too::Utilities::Color color_tr = MY_UI_Too::Utilities::White, const MY_UI_Too::Utilities::Color color_bl = MY_UI_Too::Utilities::White,const  MY_UI_Too::Utilities::Color color_br = MY_UI_Too::Utilities::White, bool getnewbuffer=false)=0;

			virtual void DrawText_NoClip(const MY_UI_Too::Interfaces::ITexture* pTexture, const MY_UI_Too::Interfaces::IFont* font, const std::string txt,const MY_UI_Too::Utilities::Point startinpos,const  unsigned int fontsize,const  MY_UI_Too::Utilities::Color color_tl = MY_UI_Too::Utilities::White,const  MY_UI_Too::Utilities::Color color_tr = MY_UI_Too::Utilities::White,const  MY_UI_Too::Utilities::Color color_bl = MY_UI_Too::Utilities::White, const MY_UI_Too::Utilities::Color color_br = MY_UI_Too::Utilities::White, bool getnewbuffer=false)=0;

			virtual MY_UI_Too::Utilities::Point Measure_String(const MY_UI_Too::Utilities::Point skinsize, const MY_UI_Too::Interfaces::IFont* font,const  unsigned int fontsize,const  std::string text)=0;

			virtual void StartClip(MY_UI_Too::Utilities::Rect& rect)=0;
			virtual void EndClip()=0;

			virtual MY_UI_Too::Interfaces::ITexture* LoadTexture(std::string filename, bool as_rendertarget=false)=0;
			virtual MY_UI_Too::Interfaces::ITexture* CreateTexture(unsigned int width, unsigned int height, Utilities::Color* buffer=nullptr, bool as_rendertarget=false)=0;
			virtual void Set_Size(MY_UI_Too::Utilities::Point screensize) =0;// this is called when the screen sets a new window dimensions
			virtual MY_UI_Too::Utilities::Point Get_Size() =0;// get the current size of the renderer window
			virtual unsigned int GetDrawCalls()const =0;

			virtual void Set_Render_Target(MY_UI_Too::Interfaces::ITexture* texture)=0;
		protected:
					
			virtual void Draw(MyDrawState& drawstate)=0;
			virtual bool SetTexture(const MY_UI_Too::Interfaces::ITexture* pTexture, bool getnewbuffer)=0;
			virtual void AddVert( float x, float y, float u, float v, Utilities::Color col)=0;
			virtual void GotoNextBufferSlot()=0;

			virtual void Get_Render_States() =0;
			virtual void Restore_Render_States() =0;
		};
	};

};

#endif
