#ifndef MY_UI_TOO_ISKIN_H
#define MY_UI_TOO_ISKIN_H
#include <string>
#include "ITexture.h"

namespace MY_UI_Too{

	class ISkin {
	public:

		virtual ~ISkin()=0;

		virtual Utilities::UVs Get_Down_Button() const=0;
		virtual Utilities::UVs Get_Up_Button() const=0;
		virtual Utilities::UVs Get_Hovered_Button() const =0;


	};

};


#endif