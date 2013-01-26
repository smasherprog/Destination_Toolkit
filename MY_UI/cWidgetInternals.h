#ifndef CWIDGETINTERNALS_H
#define CWIDGETINTERNALS_H
#include "cWidgetUtilities.h"
#include <vector>
#include <set>
#include "cWidgetEvents.h"
#include <memory>

namespace MY_UI{
	
	namespace Controls{
		// forward delcaration
		class Button;
		class cWidgetBase;
		class Image;
		class Text;
		class TextBox;
		class ColoredRect;
		class ApplicationLayer;
	};
	
	class cWidgetRenderer;
	class cRootWidget;
	class cWidgetSkin;
	bool Init(cWidgetRenderer* renderer, cWidgetSkin* skin, cRootWidget* root);
	void DeInit();
	Controls::ApplicationLayer* GetApplicationLayer();
	void UnBindKey(unsigned char key);
	void UnBindKeys(unsigned char key1, unsigned char key2);
	void BindToKey(unsigned char key, std::function<void ()> functiontocall);
	void BindToKeys(unsigned char key1, unsigned char key2 , std::function<void ()> functiontocall);
	
	template<typename T>void Safe_Delete(T& widget){
		if(widget == 0) return;
		T temp(widget);
		widget =0;// set to zero in any case
		if(Internal::AllWidgets.find(temp)==Internal::AllWidgets.end()) return;
		delete temp;
		//the widget will remove itself from the AllWidgets list in its destructor
	}
	
	namespace Internal{
		// these thre externs are the UI's internal globals needed for rendering, and processing. This is a better way than trying to insert these 
		//as static members in every different data type, which can get ugly if I have 10 different data types and I need to ensure all are set correctly..

		extern cWidgetRenderer* WidgetRenderer;
		extern cWidgetSkin* WidgetSkin;
		extern cRootWidget* RootWidget;
		extern std::set<MY_UI::Controls::cWidgetBase*> AllWidgets;
		#define POPUPTIME 600 // this is in miliseconds
		class cInternal_Widget{// I use this to hide clutter from the user so there isnt a bunch of protected variables that the user doesnt need to see or access directly
		public:
		
			Controls::Image* Selected;// this will add a background to the selected area
			Utilities::Color Color_Top_Left, Color_Top_Right, Color_Bottom_Left, Color_Bottom_Right;
			bool Hidden, Focus, Needs_Update, Draggable;//focus can be direct focus, or indirect by a child having focus
			Controls::cWidgetBase* Parent;
			int Alignment;// the alignment of the control within its parents ClientBounds
			int Buffer;
			std::vector<Controls::cWidgetBase*> Children;
			Utilities::Rect AbsolutePosition;//this is the absolute position, not a relative one
			Utilities::Rect Clientpos;// this is the postion and size where children are allowed to start living
			Utilities::Point MinSize;
			std::string Name, Popup;// this is what I use for testing purposes to see what control I am hitting
			unsigned int Id;// this is not used for anything internally. Set it to any value. It can be used as an identifier 
			void* ExtraData;// use this to pass around data. Just make sure to free any resources you need 
			unsigned int LastLeftClickTime, LastRightClickTime;
			cInternal_Widget(): Draggable(false), Selected(0), Parent(0), Focus(false), Hidden(false), Needs_Update(false), Id(0), Alignment(NO_ALIGNMENT), Buffer(0), ExtraData(0), LastLeftClickTime(0), LastRightClickTime(0) { }
			~cInternal_Widget() {}
		};
	
		class cInternal_Window{// this is to hide information from the user and cut down on the clutter in the class
		public:

			Utilities::Point Old_Size;
			Utilities::Point Old_Pos;

			bool Minimized, Maximized, Closeable, Minimizeable, Maximizeable;

			cInternal_Window(): Minimized(false), Maximized(false), Closeable(true), Minimizeable(true), Maximizeable(true) {}
		};
		class cInternal_CollapsibleList{// this is to hide information from the user and cut down on the clutter in the class
		public:

			Utilities::Point Old_Size;
			Utilities::Point Old_Pos;

			bool Minimized, Minimizeable;

			cInternal_CollapsibleList(): Minimized(false), Minimizeable(true) {}
		};
		class cInternal_Scrollable{
		public:
			Controls::Button* TR_Up_Button, *BR_Down_Button, *BR_Right_Button, *BL_Left_Button, *BR_Corner;
			Controls::Button* Right_Scroller, *Bottom_Scroller;
			Controls::Image* Right_Scroller_Background, *Bottom_Scroller_Background;
			Utilities::Point Position, Size;// the position and size of the underlying scrollable area
			float RScroller_Pos_Percent;
			cInternal_Scrollable() {  memset(this, 0, sizeof(cInternal_Scrollable)); } 
		};
	
		class cInternal_Text{// this is to hide information from the user and cut down on the clutter in the class
		public:

			bool Bold;
			std::string Text;
			std::string* LinkedText;
			float TextSize;// font size
			Utilities::Point MaxTextSize;// max size the text is allowed to goto
			cInternal_Text() : TextSize(15.0f), Bold(false), LinkedText(0) {}
		};
		#define CARETFLASHRATE 600 // this is in miliseconds
		class cInternal_TextBox{// this is to hide information from the user and cut down on the clutter in the class
		public:

			unsigned int CaretTimer, CaretIndex;
			int MaxCharacters;
			bool PasswordBox;
			cInternal_TextBox(): CaretTimer(0), CaretIndex(0), MaxCharacters(999999), PasswordBox(false) {}
		};
		template<class T>class cInternal_Numeric_TextBox{// this is to hide information from the user and cut down on the clutter in the class
		public:

			T* Users_Value;
			T Last_Value, Max, Min, Increment;
			Controls::Button* Plus, *Minus, *Val_Adj;
			cInternal_Numeric_TextBox(): Users_Value(0), Last_Value(239239232), Max(127), Min(0), Increment(1), Plus(0), Minus(0), Val_Adj(0) {}
		};


		class cImage_Helper{
		public:
			MY_UI::Utilities::Texture Texture;
			Utilities::UVs Top, Bottom, Left_Middle, Right_Middle, Middle;
			int BorderXSize, BorderYSize;
			bool DrawBorder;
			bool OwnsTexture;// if this is true, the owner of this class should destroy the texture when it is destroyed
			cImage_Helper() : OwnsTexture(true), DrawBorder(false), BorderXSize(0), BorderYSize(0) {}
		};
		class cInternal_Image{
		public:
			std::vector<cImage_Helper> Textures;
			int ImageShown;// index of the current image that should be displayed
		
			cInternal_Image() : ImageShown(-1) {}// -1 means there are no textures to display
		};
		class cInternal_Resizable{
		public:


			Controls::Button *TopLeft, *Top, *TopRight, *Right, *BottomRight, *Bottom, *BottomLeft, *Left;
			cInternal_Resizable(){ memset(this, 0, sizeof(cInternal_Resizable)); }
	
		};

		class cInternal_CheckBox{
		public:
			bool Checked, *Users_Checked;
			cInternal_CheckBox() : Checked(false), Users_Checked(0) {}
		};

		class cInternal_RadioButtonContainer{
		public:
			int ButtonAlignment, LabelAlignment;
			cInternal_RadioButtonContainer(): ButtonAlignment(BOTTOM | CENTER), LabelAlignment(LEFT | CENTER) {}
		};
		template<class T>class cInternal_Slider{
		public:
			unsigned int Precision;
			T* Data, Min, Max, Increment, LastSetValue;
			Controls::Button* Arrow, *Slider;
			Controls::Text* Text_Value;
			bool ShowText_Value;
			float PerPixelValue;
			int  Slider_Length;
			cInternal_Slider() : Increment(1), PerPixelValue(0.0f), Arrow(0), Slider(0), Data(0), Min(0), Max(255),Text_Value(0), ShowText_Value(true), Precision(4), LastSetValue(0), Slider_Length(100) {}
		};

		class cInternal_DropDown{
		public:
			bool Opened;
			Controls::Image* Background;
			Controls::Button* PlaceHolder, *DownArrow;
			unsigned int* Link;
			cInternal_DropDown() : Opened(false), PlaceHolder(0), DownArrow(0), Background(0), Link(0) {}

		};
		class cInternal_ColorPicker{
		public:

			unsigned int Red, Green, Blue;
			Controls::Image *Background;
			Controls::ColoredRect* ColorArea, *PickedColor;
			Controls::Button* Circle, *Apply, *Cancel, *RightArrow, *LeftArrow;
			Controls::ColoredRect* ColorStrip_0, *ColorStrip_1, *ColorStrip_2, *ColorStrip_3, *ColorStrip_4, *ColorStrip_5;
			
			Utilities::Color* Users_Color;

			cInternal_ColorPicker() { memset(this, 0, sizeof(cInternal_ColorPicker)); }

		};

	
	};
};
#endif