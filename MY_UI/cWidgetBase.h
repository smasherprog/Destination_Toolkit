#ifndef cWidgetBase_H
#define cWidgetBase_H
#include <vector>
#include "cWidgetInternals.h"
#include "cWidgetEvents.h"
#include "cWidgetUtilities.h"
#include <exception>
#include <assert.h>

#define DELTA_BEFORE_RAPID_FIRE 400 // 400 ms before a held key starts rapid firing
#define FIRED_KEYS_PER_SECOND 7 // the number of keys fired when held per second
#define KEYS_PER_SEC 1000/FIRED_KEYS_PER_SECOND   // how many miliseconds before a key is fired

/*
I am trying to follow a set / get philosphy to make finding functions easier and to make it clear what is actually occuring.
I could have went with a function setup like

virtual bool Hidden(){ return Internal.Hidden; }
virutal void Show(){ Internal.Hidden = true; }
virtual void Hide() {// do hide code here }

But, I started to actually look at the code and it was not organized as I would like. I want the functions to be clear in what they do
and for people to see a function name and instantly be sure of what is happening. Most functions have an opposite Get/Set 

*/

namespace MY_UI{
	namespace Controls {

		class cWidgetBase: public Internal::cWidgetHandler{
		public:
		
			Internal::cInternal_Widget Internal_Widget;// I use this to hide clutter from the user
		
			// I make the widgets 10 x 10.. why?
			cWidgetBase(cWidgetBase* parent);
			virtual ~cWidgetBase();
			//nothing special, each widget can have a name
			void SetName(std::string name){ Internal_Widget.Name =name; }
			std::string GetName()  const { return Internal_Widget.Name;  }
			// just an identifier that can be assigned for any reason. This is not used internally at all by the library. I have found it useful in some areas
			void SetId(unsigned int id) {Internal_Widget.Id = id; }
			unsigned int GetId()  const { return Internal_Widget.Id; }

			virtual void SetFocus(bool focus);
			virtual bool GetFocus() const { return Internal_Widget.Focus; }

			virtual void SetDraggable(bool drag) { Internal_Widget.Draggable = drag; }
			virtual bool GetDraggable() const { return Internal_Widget.Draggable; }

			//Hidden controls whether the object will be drawn or not. 
			virtual bool GetHidden() const { return Internal_Widget.Hidden; }
			virtual void SetHidden(bool hidden);
			// sets the total area of the control and this is the clipping area of the control.
			virtual void SetControlBounds(Utilities::Rect r);
			virtual Utilities::Rect GetControlBounds() const;
			
			virtual void SetClientBounds(Utilities::Rect r);
			virtual Utilities::Rect GetClientBounds() const;

			virtual void SetPopup(std::string popup) { Internal_Widget.Popup = popup; }
			virtual std::string GetPopup() const { return Internal_Widget.Popup; }

			virtual void SetPos(int x, int y);
			virtual void SetPos(MY_UI::Utilities::Point pos){ SetPos(pos.x, pos.y); }
			virtual MY_UI::Utilities::Point GetPos() const;
			virtual MY_UI::Utilities::Point GetAbsolutePos() const{ return MY_UI::Utilities::Point(Internal_Widget.AbsolutePosition.left, Internal_Widget.AbsolutePosition.top); }

			virtual void SetSize(int x, int y);
			virtual void SetSize(MY_UI::Utilities::Point size) { SetSize(size.x, size.y); }
			virtual MY_UI::Utilities::Point GetSize() const;

			virtual void SetLength(int x){ SetSize(x, GetSize().y); } // the length in pixels
			virtual int GetLength() const { return GetSize().x; } // the length  in pixels

			virtual void SetHeight(int y){ SetSize(GetSize().x, y); } // the height in pixels
			virtual int GetHeight() const { return GetSize().y; } // the height  in pixels

			virtual void SetMinSize(int x, int y) { Internal_Widget.MinSize = Utilities::Point(x, y); }
			virtual MY_UI::Utilities::Point GetMinSize() const { return Internal_Widget.MinSize; }

			virtual void SetColor(Utilities::Color color){ SetColors(color, color, color, color); }
			virtual Utilities::Color GetColor() const	 { return Internal_Widget.Color_Top_Left; }// if all the corners are different, thats your fault!!

			virtual void SetColors(Utilities::Color tl, Utilities::Color tr, Utilities::Color bl, Utilities::Color br);
			virtual void GetColors(Utilities::Color& tl, Utilities::Color& tr, Utilities::Color& bl, Utilities::Color& br);

			virtual void SetSelectable(bool selectedable= false);
			virtual bool GetSelectable() const { return Internal_Widget.Selected != 0; }

			virtual void Align_Within_Parent(int align, int buffer =0);
			virtual unsigned int Get_Parent_Alignment() const;

			// will attempt to call SetSize using the smallest size neccessary fitting any child objects within the bounds of the object.
			//Be carefull, calling this on a widget that has no children is the same as calling SetSize(0, 0)
			virtual void SetSizeToContents();
			virtual MY_UI::Utilities::Point GetSizeOfContents() const;
			// iterates through the children to return the largest bounding volume for all the children
			virtual MY_UI::Utilities::Rect GetLargestChildBounds() const;

			virtual void Align_To_Widget(cWidgetBase* widget, int align, int buffer =0);

			virtual cWidgetBase* Hit();
			virtual cWidgetBase* Hit_And_SetFocus();

			virtual void LeftMouseButtonDown();
			virtual void RightMouseButtonDown();

			virtual void LeftMouseButtonUp();
			virtual void RightMouseButtonUp();
			
			virtual void MouseEntered();
			virtual void MouseExited();

			virtual void MouseMoved();
			virtual void MouseWheelMoved();
			// the key that is being either released or pressed is accessable in a global variable, called Current_Key in the WidgetUtilities file. I use that instead of passing it around. 
			virtual void KeyUp();
			virtual void KeyDown();

			virtual void Draw_Clip(); 
			virtual void Draw_NoClip();
			// the difference in the force and non force functions is that Update may or maynot update a control. The control itself will decide if an update is required.
			//the forceupdate will make the control update itself
			virtual void Update();// run standard update on children
			virtual void ForceUpdate();// if a force update needs to occur, then run this

			virtual void DeCouple(cWidgetBase* child);
			virtual void Couple(cWidgetBase* child);

			//DELETES ALL CHILDREN !>!>!>!>!>!>
			virtual void clear();

			MY_UI::Internal::cWidgetCaller OnHoverEnter, OnHoverExit;
			MY_UI::Internal::cWidgetCaller OnLeftClicked, OnLeftDrag, OnLeftDoubleClick;
			MY_UI::Internal::cWidgetCaller OnRightClicked, OnRightDrag, OnRightDoubleClick;
			MY_UI::Internal::cWidgetCaller OnKeyUp, OnKeyDown;
			MY_UI::Internal::cWidgetCaller OnMouseWheelMoved;
			MY_UI::Internal::cWidgetCaller OnFocusChange;
			MY_UI::Internal::cWidgetCaller OnDestroyed;
			
		};
		// this is a class to hold objects but never itself gain focus, children can however. use this purely a container class
		class cWidget_Dummy : public cWidgetBase{
		public:

			cWidget_Dummy(cWidgetBase* parent) : cWidgetBase(parent) {}
			virtual ~cWidget_Dummy() {}
			virtual cWidgetBase* Hit(){
				cWidgetBase* controlhit = cWidgetBase::Hit();
				if(controlhit == this) return 0;
				return controlhit;
			}
			virtual cWidgetBase* Hit_And_SetFocus(){
				cWidgetBase* controlhit = cWidgetBase::Hit_And_SetFocus();
				if(controlhit == this) return 0;
				return controlhit;
			}

		};
		template<class T>class cWidget_w_Label : public  cWidgetBase {
		public:
					
			MY_UI::Controls::Text* Label;
			T* Widget;

			cWidget_w_Label(cWidgetBase* parent): cWidgetBase(parent) { 
				SetControlBounds(Utilities::Rect(0, 0, 100, 30));
				SetClientBounds(Utilities::Rect(0, 0, 100, 30));
				Label = new MY_UI::Controls::Text(this);	
				Widget = new T(this);
				SetLabel("Label", NO_ALIGNMENT);
			}
			virtual ~cWidget_w_Label(){}
/*-----------------------------Derived Virtual Functions go here -----------------------------------------*/
			virtual void SetPopup(std::string popup) { Label->SetPopup(popup); Widget->SetPopup(popup); Internal_Widget.Popup = popup;}

			virtual void Draw_Clip(){ Widget->Draw_Clip(); Label->Draw_Clip(); } 
			virtual void Draw_NoClip(){ Widget->Draw_NoClip(); Label->Draw_NoClip(); } 
/*-----------------------------Added Functions go here -----------------------------------------*/
				
			virtual void SetLabel(std::string label, int align = LEFT | IN_LINE, int buffer = 0){
				Label->SetText(label);// set the text
				Label->SetSizeToContents();// get the smallest bounds for the text
				Label->Align_To_Widget(Widget, align, buffer);
				SetSizeToContents();
			}
		};
	};
	

	class cWidgetSkin;
	namespace Controls {
		class Window;
		class Text;
		class ColoredRect;
		class ApplicationLayer;
	};
	class cRootWidget: public Controls::cWidgetBase {
		// make sure there are no copies made!
		cRootWidget(const cRootWidget& rhs);
		cRootWidget& operator=(const cRootWidget& rhs);
	public:

		cWidgetBase* FocusHolder;// the current focus holder
		cWidgetBase* HoveredWidget;// the current widget that the mouse is hovering over
		cWidgetBase* DraggedWidget;// if a widget is being dragged, this is it!

		//The children will maintain their order in the applicationlayer. So, if you insert, the a camera first, then a terrain. The order will be camera, then terrain for all checks unless you remove the children and re order them
		// the checks will be done AFTER the UI has been checked, NEVER FIRST. 
		Controls::ApplicationLayer* AppLayer;// this is a special control that holds the applications UI controls, which would be layered UNDER THE MY_UI, always. Read the Desc in the class 
		Controls::cWidget_w_Label<Controls::ColoredRect>* HoveredText;// if a user holds a mouse cursor over a control, the control might have some text it wants to appear.
		Controls::Image* Caret;
		unsigned int LastCaretFlash, TimeOverControl;
		bool WidgetHasChanged;

		std::vector<cWidgetBase*> DelayedDeleteBuffer;// Widgets can destroy themselves, therefore, deletes do not occur immediately, but at the beginning of the next loop
		// instead of running update on ALL widgets --which is slow--, I add them here to be checked
		std::vector<cWidgetBase*> WidgetsUpdateBuffer;// this is an array of widgets that need to be updated. i.e. a slider that is linked needs to be checked every loop to see if the value has changed.
		std::set<cWidgetBase*> WidgetUpdateSet;// this is just to prevent widgets from inserting themselves multiple times from derived classes
		
		std::function<void ()> KeyMap[255][255];
	

		cRootWidget();
		virtual ~cRootWidget();
		bool Init();

		// I had to add the extraparam so I could overload the function names so i dont make virtual calls. the param is never used
		bool MouseButtonUp(void (MY_UI::Controls::cWidgetBase::*func)());
		bool MouseButtonDown(void (MY_UI::Controls::cWidgetBase::*func)());

		bool MouseMoved(bool extraparam);
		bool MouseWheelMoved(bool extraparam);
		bool Key(void (MY_UI::Controls::cWidgetBase::*func)(), int key1, int key2);
		//single key events are mapped to [0][key]

		void UnBindKey(unsigned char key){ UnBindKeys(0, key); }
		void UnBindKeys(unsigned char key1, unsigned char key2){  KeyMap[key1][key2] = std::function<void()>(); }
		void BindToKey(unsigned char key, std::function<void ()> functiontocall){KeyMap[0][key] = functiontocall; }
		void BindToKeys(unsigned char key1, unsigned char key2, std::function<void ()> functiontocall){
			// there should be nothing bound to key1 because it will cause strange things to happen
			// imagine binding a function to ctrl that does something. Then, imagine binding a function to ctrl + r. Each time you goto press ctrl +r, then  ctrl function will fire off as well
			// you can bind the keys to be pressed in reverse order and it will work, like r + ctrl
			assert(KeyMap[0][key1] ==0);
			KeyMap[key1][key2]=functiontocall;
		}

		void Draw() { Draw_NoClip( ); }
		virtual void Draw_NoClip();
		virtual void Update();

		virtual void SetSize(int x, int y);
		virtual cWidgetBase* Hit();
		virtual cWidgetBase* Hit_And_SetFocus();

		void AllLoseFocus();// forces all controls in the UI to lose focus
		void CleanLinks(cWidgetBase* widget);// this will remove the widget if it has focus, or any other references
		void DelayedDelete(cWidgetBase* widget);// this is called when a widget has destroyed itself
		void DeleteAllWidgets();// this will delete (as in free any allocated memory) all widgets in existence

		void CheckForUpdate(cWidgetBase* widget, bool check = true);// widgets call this if they want update to be ran on a per loop basis. If this is called with check = to false, the widget will be removed from the list of checked widgets
		// use this when you create children and need to insert them into the application layer
		Controls::ApplicationLayer* GetApplicationLayer(){ return AppLayer; }

	};
};

#endif