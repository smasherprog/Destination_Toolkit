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
		virtual void Remove_From_Skin(Utilities::UVs u) override;

		virtual Interfaces::ITexture* Get_Skin() const override{ return _Skin; }

		virtual Utilities::UVs Get_Down_Button() const override { return _Down_Button; }
		virtual Utilities::UVs Get_Up_Button() const override { return _Up_Button; }
		virtual Utilities::UVs Get_Hovered_Button() const override { return _Hovered_Button; }

		virtual Utilities::UVs Get_CheckBox_Checked_Hovered() const override { return _CheckBox_Checked_Hovered; }
		virtual Utilities::UVs Get_CheckBox_Checked() const override { return _CheckBox_Checked; }

		virtual Utilities::UVs Get_CheckBox_UnChecked_Hovered() const override { return _CheckBox_UnChecked_Hovered; }
		virtual Utilities::UVs Get_CheckBox_UnChecked() const override { return _CheckBox_UnChecked; }

		virtual Utilities::UVs Get_Radio_Checked_Hovered()  const override { return _Radio_Checked_Hovered; }
		virtual Utilities::UVs Get_Radio_Checked()  const override { return _Radio_Checked; }

		virtual Utilities::UVs Get_Radio_UnChecked_Hovered()  const override { return _Radio_UnChecked_Hovered; }
		virtual Utilities::UVs Get_Radio_UnChecked()  const override { return _Radio_UnChecked; }



	protected:
		Utilities::UVs _Down_Button;
		Utilities::UVs _Up_Button;
		Utilities::UVs _Hovered_Button;

		Utilities::UVs _CheckBox_Checked_Hovered;
		Utilities::UVs _CheckBox_Checked;
		Utilities::UVs _CheckBox_UnChecked_Hovered;
		Utilities::UVs _CheckBox_UnChecked;

		Utilities::UVs _Radio_Checked_Hovered;
		Utilities::UVs _Radio_Checked;
		Utilities::UVs _Radio_UnChecked_Hovered;
		Utilities::UVs _Radio_UnChecked;
	};

};


#endif