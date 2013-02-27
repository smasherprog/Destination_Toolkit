#ifndef MY_UI_TOO_DX_TEXTURE_H
#define MY_UI_TOO_DX_TEXTURE_H
#include "ITexture.h"
#include "d3d11.h"

namespace MY_UI_Too{
	namespace Utilities {
		class DX_Texture : public ITexture{
		public:

			DX_Texture(){}
			virtual ~DX_Texture(){ clear();}

			virtual void clear(){
				if(_Texture != nullptr) ((ID3D11ShaderResourceView*)_Texture)->Release();
				_Texture = nullptr;
				if(_Render_Texture != nullptr) ((ID3D11RenderTargetView*)_Render_Texture)->Release();
				_Render_Texture = nullptr;
			}
		};
	};
};
#endif