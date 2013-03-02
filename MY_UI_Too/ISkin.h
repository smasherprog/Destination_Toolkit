#ifndef MY_UI_TOO_ISKIN_H
#define MY_UI_TOO_ISKIN_H
#include <string>
#include "ITexture.h"

namespace MY_UI_Too{
	class IRenderer;
	class ISkin {
	public:

		virtual ~ISkin(){}
		virtual void Init(IRenderer* renderer)=0;
		virtual void DeInit()=0;

		virtual Utilities::ITexture* Get_Skin() const=0;

		virtual Utilities::UVs Get_Down_Button() const=0;
		virtual Utilities::UVs Get_Up_Button() const=0;
		virtual Utilities::UVs Get_Hovered_Button() const =0;


	};

};


#endif