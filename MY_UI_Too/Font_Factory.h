#ifndef MY_UI_TOO_FONT_FACTORY_H
#define MY_UI_TOO_FONT_FACTORY_H
#include "IFont_Factory.h"
#include <string>
#include <map>

#pragma comment( lib, "freetype_Lib" )
typedef struct FT_LibraryRec_  *FT_Library;
typedef struct FT_FaceRec_*  FT_Face;

namespace MY_UI_Too {
	namespace Interfaces{
		class IFont;
	};

	class Font_Factory: public Interfaces::IFont_Factory{
		std::map<std::string, std::string> _Installed_Fonts;
		std::map<std::string, Interfaces::IFont*> _Fonts;
		FT_Library _Library;

	public:
		virtual ~Font_Factory(){ DeInit(); }

		virtual bool Init();
		virtual void DeInit();

		virtual std::vector<std::string> GetInstalledFonts() override;

		virtual void BuildInstalledFonts() override;// building font lists can be expensive so this should be done only when really needed
		virtual Interfaces::IFont* Get_Font(std::string fontname) override;

	};
};


#endif