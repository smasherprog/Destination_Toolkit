#ifndef MY_UI_RENDERERS_OPENGL_H
#define MY_UI_RENDERERS_OPENGL_H
#include "cWidgetRenderer.h"
#include "cWidgetUtilities.h"

#pragma comment(lib,"opengl32")

//I was having trouble with the 32 bit and 64 bit builds for Glew, so I added it as a project to compile the libaries myself. It works perfect now. This is why the Library is named differently so there
//is no confusion that its my library. GLEW STATIC is needed because this is a static libray build, no DLL is needed. The code is compiled into the exe
#define GLEW_STATIC
#include "../Glew_Lib/glew.h"
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))
#pragma comment(lib,"Glew_Lib")

#define FREEIMAGE_LIB
#include "..\FreeImage\Source\FreeImage.h"
#pragma comment(lib,"FreeImage")

struct VERTEXFORMAT2D
{
	float x, y;
	unsigned int color;
	float u, v;
};
#define VERT_BUFFER_SIZE 32768

class MyDrawState{
public:
	MyDrawState() : Verts(VERT_BUFFER_SIZE), texture(0), NumVerts(0) {}
	std::vector<VERTEXFORMAT2D>	Verts;
	void* texture;
	unsigned int NumVerts;
};

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(std::string vShaderFile, std::string fShaderFile);

namespace MY_UI{
	class OpenGL_Renderer : public MY_UI::cWidgetRenderer
	{
	public:
		OpenGL_Renderer();
		~OpenGL_Renderer();

		virtual bool Init();
		virtual void DeInit();

		virtual void Begin();
		virtual void End();
		// start new batch should be called if the previous contents of all the buffered data are no longer valid and the entire buffer should be rebuilt
		virtual void StartNewBatch(){ Draw_State_Index=0; for(size_t i(0); i < Draw_States.size(); i++) Draw_States[i].NumVerts=0;   }
		virtual void DrawBufferd();// this will draw everything that has been buffered so far
		virtual void DrawBack(bool popback);// this will draw the last valid slot RIGHT NOW... 

		// the four draw functions below only buffer the data, they do not actually draw anything until DrawBuffred is called
		virtual void DrawTexturedRect_Clip( MY_UI::Utilities::Texture& Texture,  MY_UI::Utilities::UVs& uvs, MY_UI::Utilities::Rect rect, MY_UI::Utilities::Color color_tl = MY_UI::Utilities::White, MY_UI::Utilities::Color color_tr = MY_UI::Utilities::White, MY_UI::Utilities::Color color_bl = MY_UI::Utilities::White, MY_UI::Utilities::Color color_br = MY_UI::Utilities::White, bool getnewbuffer = false);
		virtual void DrawTexturedRect_NoClip( MY_UI::Utilities::Texture& Texture,  MY_UI::Utilities::UVs& uvs, MY_UI::Utilities::Rect rect, MY_UI::Utilities::Color color_tl = MY_UI::Utilities::White, MY_UI::Utilities::Color color_tr = MY_UI::Utilities::White, MY_UI::Utilities::Color color_bl = MY_UI::Utilities::White, MY_UI::Utilities::Color color_br = MY_UI::Utilities::White, bool getnewbuffer = false);
		// user must pass a white texture to be used for the line drawing . . . .
		virtual void DrawLine_Clip( MY_UI::Utilities::Texture& pTexture, MY_UI::Utilities::Point beg, MY_UI::Utilities::Point end, int thickness = 4, MY_UI::Utilities::Color color_beg = MY_UI::Utilities::White, MY_UI::Utilities::Color color_end = MY_UI::Utilities::White, bool getnewbuffer = false);
		virtual void DrawLine_NoClip( MY_UI::Utilities::Texture& pTexture, MY_UI::Utilities::Point beg, MY_UI::Utilities::Point end, int thickness = 4, MY_UI::Utilities::Color color_beg = MY_UI::Utilities::White, MY_UI::Utilities::Color color_end = MY_UI::Utilities::White, bool getnewbuffer = false);

		virtual bool LoadTexture( MY_UI::Utilities::Texture* pTexture );
		virtual void FreeTexture( MY_UI::Utilities::Texture* pTexture );

		virtual void StartClip(MY_UI::Utilities::Rect& rect);
		virtual void EndClip();
		// the onresize does as much pre calculations are possible by divding and multiplying by two
		virtual void OnResize(unsigned int x, unsigned int y) { Inv_Wndx = (1.0f/static_cast<float>(x))*2.0f; Inv_Wndy = (1.0f/static_cast<float>(y))*-2.0f; }


	protected:

		void Draw(const MyDrawState& drawstate);
		bool SetTexture(MY_UI::Utilities::Texture& pTexture, bool getnewbuffer);
		void AddVert( float x, float y, float u, float v, Utilities::Color col);
		void GotoNextBufferSlot();


		std::vector<MY_UI::Utilities::Rect> ClipRects;
		float				Inv_Wndx, Inv_Wndy;// screen size in pixels

		std::vector<MyDrawState>  Draw_States;

		unsigned int				Draw_State_Index;

		void*						CurrentTexture;
		GLuint						GL_Shader;
		GLuint						Vertex_Buffer, Index_Buffer;
		GLuint						pos_loc;
		GLuint						color_loc;
		GLuint						tex_loc;
		GLuint						texture_loc;

	};

};
#endif
