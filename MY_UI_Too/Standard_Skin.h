#ifndef MY_UI_TOO_SKIN_H
#define MY_UI_TOO_SKIN_H
#include "ITexture.h"
#include "ISkin.h"
#include "../Utilities/Utilities.h"
#include <vector>

namespace MY_UI_Too{
	class IRenderer;
	class Standard_Skin : public ISkin {
	
		Utilities::ITexture* _Skin;
		IRenderer* _Renderer;
		std::vector<Utilities::ITexture*> _Textures;
	public:

		Standard_Skin():_Skin(nullptr), _Renderer(nullptr) {}
		virtual ~Standard_Skin() override { DeInit(); }
		
		virtual void Init(IRenderer* renderer)override;
		virtual void DeInit() override;

		virtual Utilities::UVs Get_Down_Button() const override { return _Down_Button; }
		virtual Utilities::UVs Get_Up_Button() const override { return _Up_Button; }
		virtual Utilities::UVs Get_Hovered_Button() const override { return _Hovered_Button; }
		virtual Utilities::ITexture* Get_Skin() const override{ return _Skin; }

	protected:
		Utilities::UVs _Down_Button;
		Utilities::ITexture* _Down_Button_Tex;
		Utilities::UVs _Up_Button;
		Utilities::ITexture* _Up_Button_Tex;
		Utilities::UVs _Hovered_Button;
		Utilities::ITexture* _Hovered_Button_Tex;
	};

};


#endif