#ifndef MY_UI_RENDERERS_DirectX11_H
#define MY_UI_RENDERERS_DirectX11_H
#include "cWidgetRenderer.h"
#include "cWidgetUtilities.h"

#pragma warning(push)
#pragma warning( disable : 4005 )
#include "d3d11.h"
#include "D3DX11async.h"
#include "D3DCompiler.h"
#pragma warning(pop)



#if defined(_MSC_VER)

#pragma comment(lib, "d3d11")
#pragma comment(lib, "DXGI")
#pragma comment(lib, "legacy_stdio_definitions")

#if defined(_DEBUG) | defined (DEBUG)
#pragma comment(lib, "d3dx11d")
#pragma comment(lib, "dxerr") // needed for dxtrace
#else 
#pragma comment(lib, "d3dx11")
#endif

#endif


#if defined(DEBUG) | defined (_DEBUG)
#include <dxerr.h>
#pragma comment(lib, "dxerr") // needed for dxtrace
#ifndef HR
#define HR(x){																\
	HRESULT hr = (x);														\
	if(FAILED(hr)){															\
	std::cout<<DXGetErrorStringA(hr)<<std::endl<<DXGetErrorDescriptionA(hr)<<std::endl;   \
	assert(0);															\
	}																		\
		}
#endif
#endif
#ifndef HR
#define HR(x) (x)
#endif

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

inline HRESULT CompileShaderFromMemory(const std::string& strinmem, const std::string& functionname,const std::string& shadermodel, ID3DBlob** ppBlobOut ){
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromMemory(strinmem.c_str(), strinmem.size(), NULL, NULL, NULL, functionname.c_str(), shadermodel.c_str(),  dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
	if( FAILED(hr) ){
		if( pErrorBlob != NULL ){
			OUTPUT_DEBUG_MSG( (char*)pErrorBlob->GetBufferPointer() );
			pErrorBlob->Release();
		}
		return hr;
	}
	if( pErrorBlob ) pErrorBlob->Release();

	return S_OK;
}
namespace MY_UI{
	class DirectX11 : public MY_UI::cWidgetRenderer
	{
	public:
		DirectX11( ID3D11Device* pDevice, ID3D11DeviceContext* context);
		~DirectX11();

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

		ID3D11Device*		Device;
		ID3D11DeviceContext* DeviceContext;
		std::vector<MY_UI::Utilities::Rect> ClipRects;
		float				Inv_Wndx, Inv_Wndy;// screen size in pixels

		std::vector<MyDrawState>  Draw_States;

		unsigned int				Draw_State_Index;

		void*						CurrentTexture;
		ID3D11InputLayout*			UIInputLayout;
		ID3D11PixelShader*			UIPSShader;
		ID3D11VertexShader*			UIVSShader;

		ID3D11ShaderResourceView*	UITexture;
		ID3D11Buffer*				UIVertexBuffer, *UIIndexBuffer;
		ID3D11BlendState*			UILastBlendState, *UIBlendState;
		ID3D11DepthStencilState*	UIDepthState;
		ID3D11RasterizerState*		UIRasterizerStateNormal;
		ID3D11SamplerState*			UISampler;

		// below are the states on before the UI was drawn. They are all saved so they can be restored after the UI has drawn so the UI wont interfear with any other parts of the application
		float						LastBlendFactor[4];
		UINT						LastBlendMask, LastStencilRef;
		ID3D11InputLayout*			LastInputLayout;
		D3D10_PRIMITIVE_TOPOLOGY	LastTopology;
		ID3D11Buffer*				LastBuffers[8];
		ID3D11Buffer*				LastIndexBuffer;
		ID3D11Buffer*				LastSOBuffers[4];
		DXGI_FORMAT					LastIndexBufferFormat;
		UINT						LastIndexBufferOffset;
		UINT						LastStrides[8], LastOffsets[8];
		ID3D11SamplerState*			LastPSSampler;
		ID3D11PixelShader*			LastPSShader;
		ID3D11VertexShader*			LastVSShader;
		ID3D11GeometryShader*		LastGSShader;
		ID3D11HullShader*			LastHSShader;
		ID3D11DomainShader*			LastDSShader;

		ID3D11DepthStencilState*	LastDepthState;
		ID3D11RasterizerState*		LastRasterizerState;



	};

};
#endif
