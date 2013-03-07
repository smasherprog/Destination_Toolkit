#include "PCH.h"
#include "Font_Factory.h"
#include "../Utilities/Utilities.h"
#include <set>
#include "Utilities.h"
#include "IWidget.h"
#include "IRenderer.h"
#include "IFont.h"
#include "ISkin.h"
#include "../FreeType_Lib/include/ft2build.h"
#include FT_FREETYPE_H

#ifdef _WIN32
#pragma comment(lib, "Dwrite")
#include <dwrite.h>
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
#define PATH_TOFONTS "c:/windows/fonts/"

void MY_UI_Too::Font_Factory::BuildInstalledFonts(){

	HKEY hKey=NULL;
	if(RegOpenKeyExA( HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
		0,
		KEY_READ,
		&hKey) != ERROR_SUCCESS)
	{
		OUTPUT_DEBUG_MSG("Error Loading fonts from windows registery, Error Code: "<<std::to_string(GetLastError()));
	}


	//First use the windows libraries to get the english fonts
	std::set<std::string> englishfonts;
	IDWriteFactory* pDWriteFactory = NULL;

	HRESULT hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pDWriteFactory)
		);

	IDWriteFontCollection* pFontCollection = NULL;

	// Get the system font collection.
	if (SUCCEEDED(hr)) hr = pDWriteFactory->GetSystemFontCollection(&pFontCollection);

	UINT32 familyCount = 0;
	// Get the number of font families in the collection.
	if (SUCCEEDED(hr)) familyCount = pFontCollection->GetFontFamilyCount();

	for (UINT32 i = 0; i < familyCount; ++i)
	{
		IDWriteFontFamily* pFontFamily = NULL;

		// Get the font family.
		if (SUCCEEDED(hr)) hr = pFontCollection->GetFontFamily(i, &pFontFamily);

		IDWriteLocalizedStrings* pFamilyNames = NULL;
		
		// Get a list of localized strings for the family name.
		if (SUCCEEDED(hr)) hr = pFontFamily->GetFamilyNames(&pFamilyNames);


		UINT32 index = 0;
		BOOL exists = false;

		wchar_t localeName[LOCALE_NAME_MAX_LENGTH];

		if (SUCCEEDED(hr))
		{
			// Get the default locale for this user.
			int defaultLocaleSuccess = GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);

			// If the default locale is returned, find that locale name, otherwise use "en-us".
			if (defaultLocaleSuccess) hr = pFamilyNames->FindLocaleName(localeName, &index, &exists);
			// if the above find did not find a match, retry with US English
			if (SUCCEEDED(hr) && !exists) hr = pFamilyNames->FindLocaleName(L"en-us", &index, &exists);
		}

		// If the specified locale doesn't exist, select the first on the list.
		if (!exists) index = 0;
		UINT32 length = 0;

		// Get the string length.
		if (SUCCEEDED(hr)) hr = pFamilyNames->GetStringLength(index, &length);

		// Allocate a string big enough to hold the name.
		wchar_t* name = new (std::nothrow) wchar_t[length+1];

		// Get the family name.
		if (SUCCEEDED(hr)) hr = pFamilyNames->GetString(index, name, length+1);
		std::wstring st = name;
		if (SUCCEEDED(hr)) englishfonts.insert(std::string(st.begin(), st.end())); 

		RELEASECOM(pFontFamily);
		RELEASECOM(pFamilyNames);
		delete [] name;
	}
	RELEASECOM(pFontCollection);
	RELEASECOM(pDWriteFactory);

//Now, open the windows registery to get the installed fonts with the corresponding filenames

	TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys=0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 

	DWORD i, retCode; 

	TCHAR  achValue[MAX_VALUE_NAME]; 
	DWORD cchValue = MAX_VALUE_NAME; 

	BYTE  datavalue[MAX_VALUE_NAME]; 
	DWORD maxdatavalue = MAX_VALUE_NAME; 

	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(
		hKey,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time 

	// Enumerate the subkeys, until RegEnumKeyEx fails.

	if (cValues) 
	{

		for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
		{ 
			maxdatavalue = cchValue = MAX_VALUE_NAME; 
			datavalue[0]=achValue[0] = '\0'; 
			retCode = RegEnumValue(hKey, i, 
				achValue, 
				&cchValue, 
				NULL, 
				NULL,
				datavalue,
				&maxdatavalue);

			if (retCode == ERROR_SUCCESS ) 
			{ 
				std::string foundf=std::string(achValue, cchValue);
				std::string::size_type index= foundf.find_last_of('(');
				if(index!= std::string::npos) foundf = foundf.substr(0, index);
				rtrim(foundf);
				if(englishfonts.find(foundf) != englishfonts.end()){
					_Installed_Fonts[foundf] = std::string((char*)datavalue, maxdatavalue);
				} 
				
			} 
		}
	}

}

#else 
std::vector<std::string> MY_UI_Too::Font_Factory::GetInstalledFonts(){
	static_assert("You must define your own version of GetInstalledFonts for non windows platforms");
	return std::vector<std::string>();
}

#endif
std::vector<std::string> MY_UI_Too::Font_Factory::GetInstalledFonts(){ 
	if(_Installed_Fonts.size()==0) BuildInstalledFonts(); 
	std::vector<std::string> fonts;
	for(auto beg = _Installed_Fonts.begin(); beg!= _Installed_Fonts.end(); beg++){
		fonts.push_back(beg->first);
	}
	return fonts;
}
bool MY_UI_Too::Font_Factory::Init(){
	bool b= FT_Init_FreeType( &_Library ) ==0;
	BuildInstalledFonts();
	return b;
}
void MY_UI_Too::Font_Factory::DeInit(){
	for(auto beg = _Fonts.begin(); beg!= _Fonts.end(); beg++) delete beg->second;
	_Fonts.clear();
	FT_Done_FreeType(_Library);
	_Library=0;
}

MY_UI_Too::Interfaces::IFont* MY_UI_Too::Font_Factory::Get_Font(std::string fontname){
	auto found = _Installed_Fonts.find(fontname);
	if(found == _Installed_Fonts.end()) {
		OUTPUT_DEBUG_MSG("Font '"<<fontname<<"', does not exist. Cannot load font");
		return nullptr;
	}
	auto foundfont = _Fonts.find(fontname);
	if(foundfont != _Fonts.end()){
		return foundfont->second;
	}
	
	std::string fontfilename = found->second;
	FT_Face _Font;
	Interfaces::IFont* retfont = new Interfaces::IFont();
	retfont->Set_Font(fontname);
	_Fonts[fontname] = retfont;
	fontname = PATH_TOFONTS + fontfilename;
	bool b =FT_New_Face( _Library, fontname.c_str(), 0, &_Font ) ==0;
	if(b) {
		unsigned int pixelheight=20;
		FT_GlyphSlot slot = _Font->glyph;
		FT_Set_Pixel_Sizes( _Font, 0, pixelheight );
		char start = 'a';
	
		while(start!='z'){
			FT_Load_Char( _Font, start, FT_LOAD_RENDER );
			Utilities::Color* charmap = new Utilities::Color[slot->bitmap.width*slot->bitmap.rows];
			for(int begcol = 0; begcol < slot->bitmap.rows; begcol++){
				for(int begrow = 0; begrow < slot->bitmap.width; begrow++){
					charmap[begcol * slot->bitmap.width + begrow]= Utilities::Color(slot->bitmap.buffer[begcol * slot->bitmap.width + begrow], 255, 255, 255);
				}
			}
			Interfaces::ITexture*tex =  Internal::Renderer->CreateTexture(slot->bitmap.width, slot->bitmap.rows, charmap);
			retfont->Set_Char_UV(start, Internal::UI_Skin->Add_To_Skin(tex));
			delete charmap;
			delete tex;
			start+=1;
		}
	} else {
		OUTPUT_DEBUG_MSG("Unknown Error. ....Could not load the font: "<<fontname);
	}
	if(_Font!=NULL){
		FT_Done_Face(_Font);
		_Font =0;
	}
	if(!b) return nullptr;
	return retfont;
}