#ifndef MY_UI_TOO_SKIN_H
#define MY_UI_TOO_SKIN_H
#include "ITexture.h"
#include "ISkin.h"
#include "../Utilities/Utilities.h"

namespace MY_UI_Too{
	class IRenderer;
	class Standard_Skin : public ISkin {
	
		Utilities::ITexture* _Skin;
		IRenderer* _Renderer;
	public:

		Standard_Skin(IRenderer* renderer);
		virtual ~Standard_Skin() override { SAFE_DELETE(_Skin);  }

		virtual Utilities::UVs Get_Down_Button() const override { return _Down_Button; }
		virtual Utilities::UVs Get_Up_Button() const override { return _Up_Button; }
		virtual Utilities::UVs Get_Hovered_Button() const override { return _Hovered_Button; }
		

	protected:
		Utilities::UVs _Down_Button;
		Utilities::UVs _Up_Button;
		Utilities::UVs _Hovered_Button;
	};

};


#endif