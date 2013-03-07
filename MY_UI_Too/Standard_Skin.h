#ifndef MY_UI_TOO_SKIN_H
#define MY_UI_TOO_SKIN_H
#include "ITexture.h"
#include "ISkin.h"
#include "../Utilities/Utilities.h"
#include <vector>

namespace MY_UI_Too{
	namespace Interfaces{
		class IRenderer;
	};
	
	class Standard_Skin : public Interfaces::ISkin {
	
		Interfaces::ITexture* _Skin;
		Interfaces::IRenderer* _Renderer;
		std::vector<Utilities::Rect> _Rects;
		void _Setup_Texture(Interfaces::ITexture* tex, Utilities::UVs in_uvs, Utilities::UVs& out_uvs);
		Utilities::Point _Get_Pos_For_Placement(unsigned int width, unsigned int height);

	public:

		Standard_Skin():_Skin(nullptr), _Renderer(nullptr) {}
		virtual ~Standard_Skin() override { DeInit(); }
		
		virtual void Init(Interfaces::IRenderer* renderer, unsigned int skin_size=1024)override;
		virtual void DeInit() override;

		virtual Utilities::UVs Add_To_Skin(Interfaces::ITexture* tex) override;

		virtual Utilities::UVs Get_Down_Button() const override { return _Down_Button; }
		virtual Utilities::UVs Get_Up_Button() const override { return _Up_Button; }
		virtual Utilities::UVs Get_Hovered_Button() const override { return _Hovered_Button; }

		virtual Interfaces::ITexture* Get_Skin() const override{ return _Skin; }

		

	protected:
		Utilities::UVs _Down_Button;
		Utilities::UVs _Up_Button;
		Utilities::UVs _Hovered_Button;

	};

};


#endif