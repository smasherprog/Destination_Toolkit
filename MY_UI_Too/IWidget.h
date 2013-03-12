#ifndef MY_UI_TOO_IWIDGET_H
#define MY_UI_TOO_IWIDGET_H
#include "Utilities.h"
#include <set>

namespace MY_UI_Too{

	namespace Interfaces{

		class IWidget{
		protected:
			
		public:

			IWidget();
			virtual ~IWidget();

			virtual void Set_Absolute_Pos(MY_UI_Too::Utilities::Point p)=0;
			virtual MY_UI_Too::Utilities::Point Get_Absolute_Pos() const =0;

			virtual void Set_Bounds(Utilities::Rect p)=0;
			virtual Utilities::Rect Get_Bounds()const =0;

			virtual void Set_Size(Utilities::Point p)=0;
			virtual Utilities::Point Get_Size()const =0;

			virtual void Set_Pos_ByOffset(Utilities::Point offset)=0;
			virtual void Set_Pos(Utilities::Point p)=0;
			virtual Utilities::Point Get_Pos()const =0;

			virtual void Align_TopLeft(int padding=5, IWidget* widget=nullptr)=0;
			virtual void Align_TopCenter(int padding=5, IWidget* widget=nullptr)=0;
			virtual void Align_TopRight(int padding=5, IWidget* widget=nullptr)=0;

			virtual void Align_BottomLeft(int padding=5, IWidget* widget=nullptr)=0;
			virtual void Align_BottomCenter(int padding=5, IWidget* widget=nullptr)=0;
			virtual void Align_BottomRight(int padding=5, IWidget* widget=nullptr)=0;

			virtual void Align_LeftTop(int padding=5, IWidget* widget=nullptr)=0;
			virtual void Align_LeftCenter(int padding=5, IWidget* widget=nullptr)=0;
			virtual void Align_LeftBottom(int padding=5, IWidget* widget=nullptr)=0;

			virtual void Align_RightTop(int padding=5, IWidget* widget=nullptr)=0;
			virtual void Align_RightCenter(int padding=5, IWidget* widget=nullptr)=0;
			virtual void Align_RightBottom(int padding=5, IWidget* widget=nullptr)=0;

			virtual void Align_TR(int padding, IWidget* widget)=0;
			virtual void Align_TL(int padding, IWidget* widget)=0;
			virtual void Align_BL(int padding, IWidget* widget)=0;
			virtual void Align_BR(int padding, IWidget* widget)=0;
			virtual void Align_Center()=0;// only works within parent

			//Input functions
			virtual void Mouse_Left_Down()=0;
			virtual void Mouse_Left_Up()=0;
			virtual void Mouse_Right_Down()=0;
			virtual void Mouse_Right_Up()=0;
			virtual void Mouse_Moved()=0;
			virtual void Mouse_Entered()=0;
			virtual void Mouse_Exited()=0;
			virtual void Mouse_Wheel_Moved()=0;

			virtual void Key_Down()=0;
			virtual void Key_Up()=0;


			//Utility Functions
			virtual IWidget* Hit()=0;
			virtual IWidget* Hit_And_SetFocus()=0;

			virtual void Add_Child(IWidget* child)=0;
			virtual void Remove_Child(IWidget* child)=0;
			virtual void RemoveAll_Children()=0;

			virtual void Set_Hidden(bool hidden)=0;
			virtual bool Get_Hidden()const =0;

			virtual void Set_Parent(IWidget* parent)=0;
			virtual IWidget* Get_Parent()const =0;

			virtual void Set_Focus(bool focus)=0;
			virtual bool Get_Focus() const =0;

			virtual void Set_Draggable(bool draggable)=0;
			virtual bool Get_Draggable() const =0;

			virtual void Set_Name(std::string name)=0;
			virtual std::string Get_Name()const =0;

			virtual void Set_Color(MY_UI_Too::Utilities::Color color)=0;
			virtual MY_UI_Too::Utilities::Color Get_Color() const =0;

			virtual void Draw()=0;
		};	
	};
	namespace Interfaces{
		class IRenderer;
		class ISkin;
		class IFont_Factory;
	};
	namespace Internal{

		extern std::set<MY_UI_Too::Interfaces::IWidget*> AllWidgets;
		extern MY_UI_Too::Interfaces::IWidget* Root_Widget;
		extern MY_UI_Too::Interfaces::IRenderer* Renderer;
		extern MY_UI_Too::Interfaces::ISkin* UI_Skin;
		extern MY_UI_Too::Interfaces::IFont_Factory* Font_Factory;
	};
	void Init(MY_UI_Too::Interfaces::IRenderer* renderer, MY_UI_Too::Interfaces::ISkin* skin, MY_UI_Too::Interfaces::IFont_Factory* fontfactory, MY_UI_Too::Interfaces::IWidget* root, unsigned int screen_width, unsigned int screen_height, unsigned int skinsize);
	void DeInit();
	void Safe_Delete(Interfaces::IWidget*& widget);
};


#endif
