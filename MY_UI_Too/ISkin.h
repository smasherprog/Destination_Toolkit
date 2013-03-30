#ifndef MY_UI_TOO_ISKIN_H
#define MY_UI_TOO_ISKIN_H
#include <string>
#include "ITexture.h"

namespace MY_UI_Too{
	namespace Interfaces{
		class IRenderer;
		class ISkin {
		public:

			virtual ~ISkin(){}
			virtual void Init(IRenderer* renderer, unsigned int skin_size=1024)=0;
			virtual void DeInit()=0;

			virtual Utilities::UVs Add_To_Skin(Interfaces::ITexture* tex) =0;
			virtual void Remove_From_Skin(Utilities::UVs) =0;

			virtual Interfaces::ITexture* Get_Skin() const=0;

			virtual Utilities::UVs Get_Down_Button() const=0;
			virtual Utilities::UVs Get_Up_Button() const=0;
			virtual Utilities::UVs Get_Hovered_Button() const =0;

			virtual Utilities::UVs Get_CheckBox_Checked_Hovered() const=0;
			virtual Utilities::UVs Get_CheckBox_Checked() const=0;

			virtual Utilities::UVs Get_CheckBox_UnChecked_Hovered() const=0;
			virtual Utilities::UVs Get_CheckBox_UnChecked() const=0;

			virtual Utilities::UVs Get_Radio_Checked_Hovered() const=0;
			virtual Utilities::UVs Get_Radio_Checked() const=0;

			virtual Utilities::UVs Get_Radio_UnChecked_Hovered() const=0;
			virtual Utilities::UVs Get_Radio_UnChecked() const=0;

			virtual Utilities::UVs Get_Window_Top() const=0;
			virtual Utilities::UVs Get_Window_Bottom() const=0;

			virtual Utilities::UVs Get_Text_Box_Focus() const=0;
			virtual Utilities::UVs Get_Text_Box_No_Focus() const=0;
			
		};
	};
};


#endif