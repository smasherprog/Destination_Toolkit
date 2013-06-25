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
		void _Draw(const MY_UI_Too::Utilities::Rect& untransformed_uvs, const MY_UI_Too::Utilities::UVs& target_uvs, const MY_UI_Too::Utilities::Rect& r) const;
	public:

		Standard_Skin():_Skin(nullptr), _Renderer(nullptr) {}
		virtual ~Standard_Skin() override { DeInit(); }

		virtual void Init(Interfaces::IRenderer* renderer, unsigned int skin_size=1024)override;
		virtual void DeInit() override;

		virtual MY_UI_Too::Utilities::UVs Add_To_Skin(MY_UI_Too::Interfaces::ITexture* tex) override;
		virtual void Remove_From_Skin(MY_UI_Too::Utilities::UVs uvs) override;

		virtual void Draw_Button(Widget_States state, MY_UI_Too::Utilities::Rect bounds) const override;
		virtual void Draw_CheckBox(Widget_States state, bool checked, MY_UI_Too::Utilities::Rect bounds) const override;
		virtual void Draw_Radio(Widget_States state,bool checked, MY_UI_Too::Utilities::Rect bounds)const override;
		virtual void Draw_WindowCloseButton(Widget_States state, MY_UI_Too::Utilities::Rect bounds) const override;
		virtual void Draw_Window_TitleBar(Widget_States state, MY_UI_Too::Utilities::Rect bounds)const override;
		virtual void Draw_Window_BackGround(Widget_States state, MY_UI_Too::Utilities::Rect bounds)const override;
		virtual void Draw_Text_Box(bool focus, MY_UI_Too::Utilities::Rect bounds) const override;
		virtual void Draw_Custom(MY_UI_Too::Utilities::UVs uvs, MY_UI_Too::Utilities::Rect bounds) const override;

		virtual void Draw_Text(MY_UI_Too::Interfaces::IFont* font, std::string txt, MY_UI_Too::Utilities::Point startinpos, unsigned int fontsize,  MY_UI_Too::Utilities::Color color_tl = MY_UI_Too::Utilities::Black) const override;
		virtual void Draw_Caret(MY_UI_Too::Interfaces::IFont* font, std::string txt, MY_UI_Too::Utilities::Point startinpos, unsigned int fontsize, unsigned int caretindex, MY_UI_Too::Utilities::Color color_tl = MY_UI_Too::Utilities::Black)const override;
		virtual MY_UI_Too::Utilities::Point Measure_String(MY_UI_Too::Interfaces::IFont* font, unsigned int fontsize, std::string text) const override;


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

		Utilities::UVs _Window_Hovered_Close_Button;
		Utilities::UVs _Window_Close_Button;
		Utilities::UVs _Window_Top;
		Utilities::UVs _Window_Bottom;

		Utilities::UVs _Text_Box_Focus;
		Utilities::UVs _Text_Box_No_Focus;

		unsigned int _Wnd_Title_Height;
		float _Border_Size;
		float _Skin_Scaling;
	};

};


#endif