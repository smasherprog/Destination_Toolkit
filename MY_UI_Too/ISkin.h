#ifndef MY_UI_TOO_ISKIN_H
#define MY_UI_TOO_ISKIN_H
#include <string>
#include "Common.h"
#include "Utilities.h"
#include "ITexture.h"

namespace MY_UI_Too{
	namespace Interfaces{
		class IRenderer;
		class ITexture;
		class IFont;
		class ISkin {
		public:

			virtual ~ISkin(){}
			virtual void Init(IRenderer* renderer, unsigned int skin_size=1024)=0;
			virtual void DeInit()=0;

			virtual MY_UI_Too::Utilities::UVs Add_To_Skin(MY_UI_Too::Interfaces::ITexture* tex) =0;
			virtual void Remove_From_Skin(MY_UI_Too::Utilities::UVs u) =0;

			virtual void Draw_Button(Widget_States state, MY_UI_Too::Utilities::Rect bounds) const=0;
			virtual void Draw_CheckBox(Widget_States state, bool checked, MY_UI_Too::Utilities::Rect bounds) const=0;
			virtual void Draw_Radio(Widget_States state,bool checked, MY_UI_Too::Utilities::Rect bounds) const=0;
			virtual void Draw_WindowCloseButton(Widget_States state, MY_UI_Too::Utilities::Rect bounds) const=0;
			virtual void Draw_Window(Widget_States state, MY_UI_Too::Utilities::Rect bounds) const=0;
			virtual void Draw_Text_Box(bool focus, MY_UI_Too::Utilities::Rect bounds) const=0;
		
			virtual void Draw_Custom(MY_UI_Too::Utilities::UVs u, MY_UI_Too::Utilities::Rect bounds) const=0;
			
			virtual void Draw_Text(MY_UI_Too::Interfaces::IFont* font, std::string txt, MY_UI_Too::Utilities::Point startinpos, unsigned int fontsize,  MY_UI_Too::Utilities::Color color_tl = MY_UI_Too::Utilities::Black) const=0;
			virtual MY_UI_Too::Utilities::Point Measure_String(MY_UI_Too::Interfaces::IFont* font, unsigned int fontsize, std::string text) const=0;

		};
	};
};


#endif