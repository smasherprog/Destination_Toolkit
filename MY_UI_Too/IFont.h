#ifndef MY_UI_TOO_IFONT_H
#define MY_UI_TOO_IFONT_H
#include <string>
#include "ITexture.h"

namespace MY_UI_Too{
	namespace Interfaces{
		class IFont{
			Utilities::UVs UVs[255];
			std::string _Font_Name;
		public:

			virtual ~IFont(){}
			virtual bool Init(){ return true;}
			virtual void DeInit(){}

			virtual bool Set_Font(std::string fontname){_Font_Name = fontname;return true;}
			virtual std::string Get_Font() const {return _Font_Name;}

			virtual void Set_Char_UV(unsigned char c, Utilities::UVs u) {  UVs[c] = u; }
			virtual const Utilities::UVs& Get_Char(unsigned char c) const { return UVs[c]; }


		};	
	};
};
#endif