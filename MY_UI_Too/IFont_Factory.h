#ifndef MY_UI_TOO_IFONT_FACTORY_H
#define MY_UI_TOO_IFONT_FACTORY_H
#include <string>
#include <vector>

namespace MY_UI_Too {
	namespace Interfaces{
		class IFont;
		class IFont_Factory{
		public:

			virtual ~IFont_Factory() {}
			virtual bool Init() =0;
			virtual void DeInit() =0;

			virtual std::vector<std::string> GetInstalledFonts()=0;//Font names
			virtual void BuildInstalledFonts()=0;// building font lists can be expensive so this should be done only when really needed

			virtual Interfaces::IFont* Get_Font(std::string fontname)=0;
		};
	};
};


#endif