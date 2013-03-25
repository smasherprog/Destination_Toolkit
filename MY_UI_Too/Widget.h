#ifndef MY_UI_TOO_WIDGET_H
#define MY_UI_TOO_WIDGET_H
#include <set>
#include "../Utilities/Signal.h"
#include "Utilities.h"
#include "IWidget.h"

namespace MY_UI_Too{
	namespace Controls{
		class Widget;
	};
	namespace Internal{
			class Internal_Widget{// I use this to hide clutter from the user so there isnt a bunch of protected variables that the user doesnt need to see or access directly
		public:

			bool Hidden, Focus, Draggable, Hitable;//focus can be direct focus, or indirect by a child having focus
			Interfaces::IWidget* Parent;

			std::vector<Interfaces::IWidget*> Children;
			Utilities::Point Size;
			Utilities::Point Pos;//relative position
			Utilities::Point Absolute_TL;//this is the absolute position, not a relative one
			std::string Name;// this is what I use for testing purposes to see what control I am hitting
			MY_UI_Too::Utilities::Color Color;

			Internal_Widget(): Draggable(false), Parent(nullptr), Focus(false), Hidden(false), Hitable(true) { }
			~Internal_Widget() {}
		};
		
	};
	
	namespace Controls {

		class Widget: public Interfaces::IWidget{

		protected:
			MY_UI_Too::Internal::Internal_Widget _Internals;
		public:

			Widget(IWidget* parent);
			virtual ~Widget() override;

			MY_Utilities::Signal_st<void> On_Destructor;

			virtual void Set_Absolute_Pos(MY_UI_Too::Utilities::Point p)override;
			virtual void Set_Absolute_Pos_ByOffset(MY_UI_Too::Utilities::Point p) override;
			virtual MY_UI_Too::Utilities::Point Get_Absolute_Pos() const override{ return _Internals.Absolute_TL; }
	
			virtual void Set_Size(Utilities::Point p) override;
			virtual Utilities::Point Get_Size()const override;

			virtual void Set_Pos_ByOffset(Utilities::Point offset) override;
			virtual void Set_Pos(Utilities::Point p) override;
			virtual Utilities::Point Get_Pos()const override;

			virtual void Align_TopLeft(int padding=5, IWidget* widget=nullptr) override;
			virtual void Align_TopCenter(int padding=5, IWidget* widget=nullptr) override;
			virtual void Align_TopRight(int padding=5, IWidget* widget=nullptr) override;

			virtual void Align_BottomLeft(int padding=5, IWidget* widget=nullptr) override;
			virtual void Align_BottomCenter(int padding=5, IWidget* widget=nullptr) override;
			virtual void Align_BottomRight(int padding=5, IWidget* widget=nullptr) override;

			virtual void Align_LeftTop(int padding=5, IWidget* widget=nullptr) override;
			virtual void Align_LeftCenter(int padding=5, IWidget* widget=nullptr) override;
			virtual void Align_LeftBottom(int padding=5, IWidget* widget=nullptr) override;

			virtual void Align_RightTop(int padding=5, IWidget* widget=nullptr) override;
			virtual void Align_RightCenter(int padding=5, IWidget* widget=nullptr) override;
			virtual void Align_RightBottom(int padding=5, IWidget* widget=nullptr) override;

			virtual void Align_TR(int padding, IWidget* widget) override;
			virtual void Align_TL(int padding, IWidget* widget) override;
			virtual void Align_BL(int padding, IWidget* widget) override;
			virtual void Align_BR(int padding, IWidget* widget) override;
			virtual void Align_Center() override;;// only works within parent

			//Input functions
			virtual void Mouse_Left_Down() override;
			virtual void Mouse_Left_Up() override;
			virtual void Mouse_Right_Down() override;
			virtual void Mouse_Right_Up() override;
			virtual void Mouse_Moved() override;
			virtual void Mouse_Entered() override;
			virtual void Mouse_Exited() override;
			virtual void Mouse_Wheel_Moved() override;

			virtual void Key_Down() override;
			virtual void Key_Up() override;

			//events that outside code can register with to call custom functions.
			MY_Utilities::Signal_st<void> On_Mouse_Left_Down;
			MY_Utilities::Signal_st<void> On_Mouse_Left_Up;
			MY_Utilities::Signal_st<void> On_Mouse_Right_Down;
			MY_Utilities::Signal_st<void> On_Mouse_Right_Up;
			MY_Utilities::Signal_st<void> On_Mouse_Moved;
			MY_Utilities::Signal_st<void> On_Mouse_Exited;
			MY_Utilities::Signal_st<void> On_Mouse_Entered;
			MY_Utilities::Signal_st<void> On_Mouse_Wheel_Moved;
			MY_Utilities::Signal_st<void> On_Key_Down;
			MY_Utilities::Signal_st<void> On_Key_Up;

			//Utility Functions
			virtual IWidget* Hit() override;
			virtual IWidget* Hit_And_SetFocus() override;

			virtual void Add_Child(IWidget* child)override;
			virtual void Remove_Child(IWidget* child)override;// this does not call delete on the child
			virtual void RemoveAll_Children()override;// this DOES call delete on all children!

			virtual void Set_Hidden(bool hidden) override {_Internals.Hidden=hidden;}
			virtual bool Get_Hidden()const override { return _Internals.Hidden;}

			virtual void Set_Parent(IWidget* parent) override { _Internals.Parent=parent;}
			virtual IWidget* Get_Parent()const override { return _Internals.Parent;}

			virtual void Set_Focus(bool focus)override {_Internals.Focus=focus;}
			virtual bool Get_Focus()const override { return _Internals.Focus;}

			virtual void Set_Draggable(bool draggable)override {_Internals.Draggable=draggable;}
			virtual bool Get_Draggable() const override { return _Internals.Draggable;}

			virtual void Set_Hitable(bool hitable)override {_Internals.Hitable=hitable;}
			virtual bool Get_Hitable() const override { return _Internals.Hitable;}

			virtual void Set_Name(std::string name) override{_Internals.Name=name;}
			virtual std::string Get_Name() const override { return _Internals.Name;}

			virtual void Set_Color(MY_UI_Too::Utilities::Color color) override{ _Internals.Color = color; } 
			virtual MY_UI_Too::Utilities::Color Get_Color()const override{ return _Internals.Color; }

			virtual void Draw()override;
		};	
	};
};


#endif
