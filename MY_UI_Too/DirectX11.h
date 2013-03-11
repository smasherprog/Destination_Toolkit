#ifndef MY_UI_RENDERERS_DirectX11_H
#define MY_UI_RENDERERS_DirectX11_H
#include "Renderer.h"
#include "../Utilities/Utilities.h"
#include <vector>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#pragma warning(push)
#pragma warning( disable : 4005 )
#include "d3d11.h"
#include "D3DX11async.h"
#include "D3DCompiler.h"
#pragma warning(pop)

#pragma comment(lib, "d3d11")
#pragma comment(lib, "DXGI")

#if defined(_DEBUG) | defined (DEBUG)
#pragma comment(lib, "d3dx11d")
#pragma comment(lib, "dxerr") // needed for dxtrace
#else 
#pragma comment(lib, "d3dx11")
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
namespace MY_UI_Too{
	class DirectX11 : public MY_UI_Too::Renderer
	{
	public:
		DirectX11( ID3D11Device* pDevice, ID3D11DeviceContext* context);
		virtual ~DirectX11() override;

		virtual bool Init() override;
		virtual void DeInit() override;
	
		virtual MY_UI_Too::Interfaces::ITexture* LoadTexture(std::string filename, bool as_rendertarget=false) override;
		virtual MY_UI_Too::Interfaces::ITexture* CreateTexture(unsigned int width, unsigned int height, Utilities::Color* buffer=nullptr, bool as_rendertarget=false) override;
		virtual void Set_Render_Target(MY_UI_Too::Interfaces::ITexture* texture)override;

	protected:
		
		virtual void Get_Render_States()override;
		virtual void Restore_Render_States()override;

		virtual void Draw(MyDrawState& drawstate) override;

		ID3D11Device*		Device;
		ID3D11DeviceContext* DeviceContext;
		
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

		ID3D11RenderTargetView*		LastRTVs[8];
		ID3D11DepthStencilView*		LastDTV;
		D3D11_VIEWPORT				LastViewPort;
		bool						ResetRT;
	};

};
#endif
