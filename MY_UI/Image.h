#ifndef CWIDGET_IMAGE_H
#define CWIDGET_IMAGE_H
#include "cWidgetBase.h"

namespace MY_UI{

	namespace Controls{
		class Image: public cWidgetBase {
		public:
			Internal::cInternal_Image Internal_Image;
			
			Image(cWidgetBase* parent): cWidgetBase(parent) { }

			virtual ~Image();
/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/

			virtual void Draw_NoClip();
			virtual void Draw_Clip();

/*-----------------------------Added Functions go here -----------------------------------------*/
			// this will clear any previous textures stored
			virtual void Set_Texture(Utilities::Texture& tex, bool takeownership = false);
			virtual bool Set_Texture(std::string filename);
			// this will add the texture the internally stored array of textures for this object
			// slot is where the texture should be added
			virtual void Add_Texture(Utilities::Texture& tex, bool takeownership = false, int slot =-1);
			virtual bool Add_Texture(std::string filename, int slot =-1);

			virtual void Set_ShownImage(unsigned int index);
			virtual unsigned int Get_ShownImage();
			virtual void SetUVs(const float u1, const float v1, const float u2, const float v2, unsigned int index=0);
			virtual void ClearImages();// this will clear all images stored, 
			virtual void ClearImage(int index);
			virtual bool ImageExists(unsigned int index );

			virtual void SetImageBorderSize(int x, int y, bool drawnborder = true, unsigned int index=9999);
			virtual MY_UI::Utilities::Point GetImageBorderSize(unsigned int index=0) const { return MY_UI::Utilities::Point(Internal_Image.Textures[index].BorderXSize, Internal_Image.Textures[index].BorderYSize); }

			virtual void SetBorderDrawn(bool drawnborder, unsigned int index=9999);
			virtual unsigned int GetBorderDrawn(unsigned int index=0) const { return Internal_Image.Textures[index].DrawBorder;}

		};
		typedef cWidget_w_Label<Image> Image_w_Label;

		class ColoredRect: public cWidgetBase {
		public:

			
			ColoredRect(cWidgetBase* parent): cWidgetBase(parent), User_Color(0) { }

			virtual ~ColoredRect(){}
/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/
			virtual void Draw_NoClip();
			virtual void Draw_Clip();

/*-----------------------------Added Functions go here -----------------------------------------*/
			// this will clear any previous textures stored
			Utilities::Color* User_Color;

		};
		typedef cWidget_w_Label<ColoredRect> ColoredRect_w_Label;

	};
};


#endif