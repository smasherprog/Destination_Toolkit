#ifndef MY_UI_TOO_IWIDGET_H
#define MY_UI_TOO_IWIDGET_H
#include "Utilities.h"
#include <set>

namespace MY_UI_Too{

	namespace Interfaces{
		class IRenderer;
		class ISkin;
		class IFont_Factory;
		class IInput;

		class IWidget{
		protected:
			
		public:

			IWidget();
			virtual ~IWidget();

			virtual void Set_Absolute_Pos(MY_UI_Too::Utilities::Point p)=0;
			virtual void Set_Absolute_Pos_ByOffset(MY_UI_Too::Utilities::Point p)=0;
			virtual MY_UI_Too::Utilities::Point Get_Absolute_Pos() const =0;

			virtual void Set_Size(Utilities::Point p)=0;
			virtual void Set_Size_To_Contents()=0;
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
			virtual void Mouse_Left_DblClk()=0;
			virtual void Mouse_Right_Down()=0;
			virtual void Mouse_Right_Up()=0;
			virtual void Mouse_Right_DblClk()=0;
			virtual void Mouse_Moved()=0;
			virtual void Mouse_Entered()=0;
			virtual void Mouse_Exited()=0;
			virtual void Mouse_Wheel_Moved()=0;

			virtual void Key_Down()=0;
			virtual void Key_Up()=0;


			//Utility Functions
			virtual IWidget* Hit()=0;
			virtual IWidget* Hit_And_SetFocus()=0;
			virtual void Delete_This()=0;// this function will delete the widget and free its memory 

			virtual void Attach_Child(IWidget* child)=0;
			virtual void Detach_Child(IWidget* child)=0;// this does not call delete on the child
			virtual void Detach_From_Parent()=0;
			virtual void DeleteAll_Children()=0;// this DOES call delete on all children!

			virtual void Set_Hidden(bool hidden)=0;
			virtual bool Get_Hidden()const =0;

			virtual void Set_Parent(IWidget* parent)=0;
			virtual IWidget* Get_Parent()const =0;

			virtual void Set_Focus(bool focus)=0;
			virtual bool Get_Focus() const =0;

			virtual void Set_Draggable(bool draggable)=0;
			virtual bool Get_Draggable() const =0;

			virtual void Set_Hitable(bool hitable)=0;
			virtual bool Get_Hitable() const =0;

			virtual void Set_Name(std::string name)=0;
			virtual std::string Get_Name()const =0;

			virtual void Set_Color(MY_UI_Too::Utilities::Color color)=0;
			virtual MY_UI_Too::Utilities::Color Get_Color() const =0;

			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin)=0;
		};	
	};

	namespace Internal{

		extern std::set<MY_UI_Too::Interfaces::IWidget*> AllWidgets;
		extern std::set<MY_UI_Too::Interfaces::IWidget*> Widgets_ToBeDeleted;
		extern MY_UI_Too::Interfaces::IWidget* Root_Widget;
		extern MY_UI_Too::Interfaces::IRenderer* Renderer;
		extern MY_UI_Too::Interfaces::ISkin* UI_Skin;
		extern MY_UI_Too::Interfaces::IInput* Input;
		extern MY_UI_Too::Interfaces::IWidget* Focus_Holder;
		extern MY_UI_Too::Interfaces::IWidget* Hovered_Widget;
		extern MY_UI_Too::Interfaces::IWidget* Dragged_Widget;
		extern MY_UI_Too::Interfaces::IFont_Factory* Font_Factory;
	};
	void Init(MY_UI_Too::Interfaces::IRenderer* renderer, MY_UI_Too::Interfaces::ISkin* skin, MY_UI_Too::Interfaces::IFont_Factory* fontfactory, MY_UI_Too::Interfaces::IWidget* root, MY_UI_Too::Interfaces::IInput* input, unsigned int screen_width, unsigned int screen_height, unsigned int skinsize);
	void DeInit();
	template<typename T>void Safe_Delete(T& widget){
		if(widget == nullptr) return;
		auto found = Internal::AllWidgets.find(widget);
		if(found!=Internal::AllWidgets.end()) delete widget;// the destructor should take care of removing itself
		widget = nullptr;
	}
};


#endif
