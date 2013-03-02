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

			bool Hidden, Focus, Draggable;//focus can be direct focus, or indirect by a child having focus
			Controls::IWidget* Parent;

			std::vector<Controls::IWidget*> Children;
			Utilities::Rect Absolute_Control_Area;//this is the absolute position, not a relative one
			Utilities::Rect Relative_Client_Area;// this is the postion and size where children are allowed to start living
			std::string Name;// this is what I use for testing purposes to see what control I am hitting

			Internal_Widget(): Draggable(false), Parent(nullptr), Focus(false), Hidden(false) { }
			~Internal_Widget() {}
		};
	};

	namespace Controls {

		class Widget: public IWidget{

		protected:
			MY_UI_Too::Internal::Internal_Widget _Internals;
		public:

			Widget(IWidget* parent);
			virtual ~Widget() override;
			MY_Utilities::Signal_st<void> On_Destructor;

			/*
			all control positions are specific in absolute coordinates. Meaning relative to the top left corner of the Control Top Left.

			*/
			virtual void Set_Control_Bounds(Utilities::Rect p) override;
			virtual Utilities::Rect Get_Control_Bounds() override;

			virtual void Set_Control_Size(Utilities::Point p) override;
			virtual Utilities::Point Get_Control_Size() override;

			virtual void Set_Control_Pos(Utilities::Point p) override;
			virtual Utilities::Point Get_Control_Pos() override;

			/*
			all client positions are specific in relative coordinates. Meaning relative to the top left corner of the Control Top Left.
			*/
			virtual void Set_Client_Bounds(Utilities::Rect p) override;
			virtual Utilities::Rect Get_Client_Bounds() override;

			virtual void Set_Client_Size(Utilities::Point p) override;
			virtual Utilities::Point Get_Client_Size() override;

			virtual void Set_Client_Pos(Utilities::Point p) override;
			virtual Utilities::Point Get_Client_Pos() override;

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

			virtual void Add(IWidget* child) override;
			virtual void Remove(IWidget* child) override;

			virtual void Set_Hidden(bool hidden) override {_Internals.Hidden=hidden;}
			virtual bool Get_Hidden()const override { return _Internals.Hidden;}

			virtual void Set_Parent(IWidget* parent) override { _Internals.Parent=parent;}
			virtual IWidget* Get_Parent()const override { return _Internals.Parent;}

			virtual void Set_Focus(bool focus)override {_Internals.Focus=focus;}
			virtual bool Get_Focus()const override { return _Internals.Focus;}

			virtual void Set_Draggable(bool draggable)override {_Internals.Draggable=draggable;}
			virtual bool Get_Draggable() const override { return _Internals.Draggable;}

			virtual void Set_Name(std::string name) override{_Internals.Name=name;}
			virtual std::string Get_Name() const override { return _Internals.Name;}

			virtual void Draw()override{}//nothing to do here
		};	
	};
};


#endif
