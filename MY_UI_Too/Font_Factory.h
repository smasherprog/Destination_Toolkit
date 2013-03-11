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
	class IFont_Wrapper{
	public:
		IFont_Wrapper() {font=nullptr; _Ref_Count=0;}
		Interfaces::IFont* font;
		int _Ref_Count;
	};
	class Font_Factory: public Interfaces::IFont_Factory{
		std::map<std::string, std::string> _Installed_Fonts;
		std::map<std::string, IFont_Wrapper> _Fonts;
		FT_Library _Library;

	protected:
		virtual Interfaces::IFont* Get_NewFont();//if you want a different IFont class, then derive a new FontFactory and simply override this function
	public:
		virtual ~Font_Factory(){ DeInit(); }

		virtual bool Init();
		virtual void DeInit();

		virtual std::vector<std::string> GetInstalledFonts() override;
		
		virtual void BuildInstalledFonts() override;// building font lists can be expensive so this should be done only when really needed
		virtual Interfaces::IFont* Get_Font(std::string fontname ="Times New Roman") override;
		virtual void Destroy_Font(std::string fontname ) override;
		//if Measure_String is called on a font that does not exist, that font will be loaded into the skin and measured. This could be expensive if called on a font not loaded.
		virtual MY_UI_Too::Utilities::Point Measure_String(std::string fontname, unsigned int fontsize, std::string text);


	};
};


#endif