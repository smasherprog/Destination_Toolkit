#ifndef CWIDGETTEXTBOX_H
#define CWIDGETTEXTBOX_H
#include "cWidgetBase.h"
#include <string>
#include <sstream>
#include "Button.h"
#include "Value_Adjuster.h"
#include "cWidgetSkin.h"
#include "cWidgetUtilities.h"

namespace MY_UI{

	namespace Controls{
		class TextBox: public  cWidgetBase {
		public:

			Internal::cInternal_TextBox Internal_TextBox;

			TextBox(cWidgetBase* parent);
			virtual ~TextBox();
/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/
			virtual void SetFocus(bool focus);

			virtual void SetSize(int x, int y);

			virtual void KeyDown();

			virtual void Draw_Clip();
			virtual void Draw_NoClip();
			virtual void SetSelectable(bool selectedable = false );
			virtual void SetPopup(std::string popup) { Internal_Widget.Popup = popup; Box->SetPopup(popup); }

/*-----------------------------Added Functions go here -----------------------------------------*/
	
			virtual void Move_Caret_To_Mouse();
			virtual void Move_Caret_To_Index();
			virtual void Move_Caret_Left();
			virtual void Move_Caret_Right();

			virtual void Select_All_Text();
			virtual void Toggle_TextCursor();

			virtual void UpdateCaret();

			virtual void SetMaxCharacters(int m) {Internal_TextBox.MaxCharacters = m; }
			virtual int GetMaxCharacters() const { return Internal_TextBox.MaxCharacters;}
			virtual void SetText(std::string txt);
			virtual std::string GetText() const ;
			virtual void Set_PasswordBox(bool p){ Internal_TextBox.PasswordBox = p; }
			virtual bool Get_PasswordBox(){ return Internal_TextBox.PasswordBox; }

			MY_UI::Controls::Text* Text;
			MY_UI::Controls::Button* Box;
			MY_UI::Internal::cWidgetCaller OnReturnPressed;

		};
		typedef cWidget_w_Label<TextBox> TextBox_w_Label;


		template<class T>class Numeric_TextBox: public TextBox {
		public:

			Internal::cInternal_Numeric_TextBox<T> Internal_Numeric_TextBox;
			Numeric_TextBox(cWidgetBase* parent): TextBox(parent) {
				Box->OnMouseWheelMoved.Add(std::bind(&MY_UI::Controls::Numeric_TextBox<T>::MouseWheelMoved, this));
			}
			virtual ~Numeric_TextBox(){}
			virtual void KeyDown(){				
				auto skin = MY_UI::Internal::WidgetSkin;
				if(Current_Key == KEY_LEFT) Move_Caret_Left();
				else if(Current_Key == KEY_RIGHT) Move_Caret_Right();
				else {
					if( (Current_Key >= KEY_0) & (Current_Key <= KEY_9) ) {
						std::string t = Text->GetText();
						if((int)t.size() +1 > Internal_TextBox.MaxCharacters) return;// do not allow more characters
						char c;
						if(ShiftDown | CapsLock) c = UpperChars[Current_Key];
						else c = LowerChars[Current_Key];
						if(skin->Keyboard[c].width + Text->GetTextSize().left > Box->GetSize().left) return;// do not allow characters to overflow the box
						t.insert(t.begin() + Internal_TextBox.CaretIndex, c);
						Text->SetText(t);
						Move_Caret_Right();
					} else if (Current_Key == KEY_BACK){
						std::string t = Text->GetText();
						if(Internal_TextBox.CaretIndex == 0 || t.size() == 0) return;
						t.erase(t.begin() + Internal_TextBox.CaretIndex -1);
						Text->SetText(t);
						Move_Caret_Left();
					}else if (Current_Key == KEY_DELETE){
						std::string t = Text->GetText();
						if(t.begin() + Internal_TextBox.CaretIndex == t.end()) return;
						t.erase(t.begin() + Internal_TextBox.CaretIndex);
						Text->SetText(t);
						UpdateCaret();
					} else if(Current_Key == KEY_UP){
						Increment();
					} else if(Current_Key == KEY_DOWN){
						Decrement();
					} else if (Current_Key == KEY_RETURN){
						std::istringstream iks(Text->GetText());
						iks>>Internal_Numeric_TextBox.Last_Value;
						Internal_Numeric_TextBox.Last_Value = Utilities::Clamp<T>(Internal_Numeric_TextBox.Last_Value, Internal_Numeric_TextBox.Min,  Internal_Numeric_TextBox.Max);
						std::ostringstream oks;
						oks<<Internal_Numeric_TextBox.Last_Value;
						Text->SetText(oks.str());
						if(Internal_Numeric_TextBox.Users_Value !=0 ) *Internal_Numeric_TextBox.Users_Value=Internal_Numeric_TextBox.Last_Value;// update the text	
						UpdateCaret();
					}
				}
				MY_UI::Controls::cWidgetBase::KeyUp();
			}
			virtual void Increment(){
				*Internal_Numeric_TextBox.Users_Value+=Internal_Numeric_TextBox.Increment;
			}
			virtual void Decrement(){
				*Internal_Numeric_TextBox.Users_Value-=Internal_Numeric_TextBox.Increment;
			}
			virtual void SetLink(T* ptr){
				Internal_Numeric_TextBox.Users_Value = ptr;	
				MY_UI::Internal::RootWidget->CheckForUpdate(this);
			}
			virtual void Update(){
				if(Internal_Numeric_TextBox.Users_Value !=0 ) {// if there is a pointer, check to see if an update is needed
					if(*Internal_Numeric_TextBox.Users_Value == Internal_Numeric_TextBox.Last_Value) return;// same value, no need to redo work
					Internal_Numeric_TextBox.Last_Value = *Internal_Numeric_TextBox.Users_Value;
					// make sure to clamp it to our min max
					Internal_Numeric_TextBox.Last_Value = Utilities::Clamp<T>(Internal_Numeric_TextBox.Last_Value, Internal_Numeric_TextBox.Min,  Internal_Numeric_TextBox.Max);
					*Internal_Numeric_TextBox.Users_Value = Internal_Numeric_TextBox.Last_Value;
					std::ostringstream oks;
					oks<<*Internal_Numeric_TextBox.Users_Value;
					Text->SetText(oks.str());
				} 
			}
			virtual void SetRange(T min, T max){ Internal_Numeric_TextBox.Min = min; Internal_Numeric_TextBox.Max = max; }
			virtual T GetValue() { return Internal_Numeric_TextBox.Last_Value; }
			virtual T SetValue(T v) { return Internal_Numeric_TextBox.Last_Value = v; }
			virtual void SetMin(T m){ Internal_Numeric_TextBox.Min =m; }
			virtual T GetMin(){ return Internal_Numeric_TextBox.Min; }
			virtual void SetMax(T m){ Internal_Numeric_TextBox.Max =m; }
			virtual T GetMax(){ return Internal_Numeric_TextBox.Max; }
			virtual void SetIncrement(T inc){ Internal_Numeric_TextBox.Increment = inc; }
			virtual T GetIncrement(){ return Internal_Numeric_TextBox.Increment; }
			virtual void MouseWheelMoved(){ 
				if(Mouse_Wheel_Delta < 0){
					for(int i=0; i< abs(Mouse_Wheel_Delta); i++) {
						Decrement();
					}
				} else {
					for(int i=0; i< Mouse_Wheel_Delta; i++) {
						Increment();
					}
				}
			}
		};


		template<class T>class Numeric_TextBox_w_Adjusters: public Numeric_TextBox<T> {
		public:

			Value_Adjuster* Val;
			Numeric_TextBox_w_Adjusters(cWidgetBase* parent): Numeric_TextBox(parent) {
				Val=0;
				auto skin = MY_UI::Internal::WidgetSkin;
				
				Internal_Numeric_TextBox.Plus = new Controls::Button(this);
				Internal_Numeric_TextBox.Plus->ClearImages();
				Internal_Numeric_TextBox.Plus->Set_Pressed_Texture(skin->Plus_Button_Pressed);
				Internal_Numeric_TextBox.Plus->Set_UnPressed_Texture(skin->Plus_Button_UnPressed);
				Internal_Numeric_TextBox.Plus->SetSize(static_cast<int>(skin->Plus_Button_UnPressed.width), static_cast<int>(skin->Plus_Button_UnPressed.height));
				Internal_Numeric_TextBox.Plus->SetImageBorderSize(2, 2);
				Internal_Numeric_TextBox.Plus->OnLeftClicked.Add(std::bind(&MY_UI::Controls::Numeric_TextBox<T>::Increment, this));
				Internal_Numeric_TextBox.Plus->OnKeyUp.Add(std::bind( &MY_UI::Controls::Numeric_TextBox<T>::KeyUp, this));

				Internal_Numeric_TextBox.Minus= new Controls::Button(this);
				Internal_Numeric_TextBox.Minus->ClearImages();
				Internal_Numeric_TextBox.Minus->Set_Pressed_Texture(skin->Minus_Button_Pressed);
				Internal_Numeric_TextBox.Minus->Set_UnPressed_Texture(skin->Minus_Button_UnPressed);
				Internal_Numeric_TextBox.Minus->SetSize(static_cast<int>(skin->Minus_Button_UnPressed.width), static_cast<int>(skin->Minus_Button_UnPressed.height));
				Internal_Numeric_TextBox.Minus->SetImageBorderSize(2, 2);
				Internal_Numeric_TextBox.Minus->OnLeftClicked.Add(std::bind( &MY_UI::Controls::Numeric_TextBox<T>::Decrement, this));
				Internal_Numeric_TextBox.Minus->OnKeyDown.Add(std::bind(&MY_UI::Controls::Numeric_TextBox<T>::KeyDown, this));

				Internal_Numeric_TextBox.Val_Adj= new Controls::Button(this);
				Internal_Numeric_TextBox.Val_Adj->ClearImages();
				Internal_Numeric_TextBox.Val_Adj->Set_Pressed_Texture(skin->Dot_Button_Pressed);
				Internal_Numeric_TextBox.Val_Adj->Set_UnPressed_Texture(skin->Dot_Button_UnPressed);
				Internal_Numeric_TextBox.Val_Adj->SetSize(static_cast<int>(skin->Dot_Button_Pressed.width), static_cast<int>(skin->Dot_Button_Pressed.height));
				Internal_Numeric_TextBox.Val_Adj->SetImageBorderSize(2, 2);
				Internal_Numeric_TextBox.Val_Adj->OnLeftDrag.Add(std::bind( &MY_UI::Controls::Numeric_TextBox_w_Adjusters<T>::OpenEditor, this));
				Internal_Numeric_TextBox.Val_Adj->SetDraggable(true);
				Internal_Numeric_TextBox.Val_Adj->OnKeyDown.Add(std::bind( &MY_UI::Controls::Numeric_TextBox<T>::KeyDown, this));
				
				Utilities::Point s = GetSize();
				SetSize(100, s.y);
			}
			virtual ~Numeric_TextBox_w_Adjusters(){}
		
			virtual void SetSize(int x, int y){
				MY_UI::Controls::TextBox::SetSize(x, y);
				Internal_Numeric_TextBox.Val_Adj->Align_Within_Parent(RIGHT | CENTER, 2);
				Internal_Numeric_TextBox.Plus->Align_To_Widget(Internal_Numeric_TextBox.Val_Adj, LEFT |IN_LINE);
				Internal_Numeric_TextBox.Minus->Align_To_Widget(Internal_Numeric_TextBox.Plus, LEFT |IN_LINE);			
			}
			virtual void Val_Adjuster_Destroyed(){
				Val =0;
			}
			virtual void OpenEditor(){		
				if(Val !=0 ) return ;
				Val = new Value_Adjuster(this);
				Val->OnIncrease.Add(std::bind(&MY_UI::Controls::Numeric_TextBox<T>::Increment, this));
				Val->OnDecrease.Add(std::bind( &MY_UI::Controls::Numeric_TextBox<T>::Decrement, this));
				Val->OnDestroyed.Add(std::bind( &MY_UI::Controls::Numeric_TextBox_w_Adjusters<T>::Val_Adjuster_Destroyed, this));
				Utilities::Point p = Internal_Numeric_TextBox.Val_Adj->GetPos();
				Utilities::Point s = Internal_Numeric_TextBox.Val_Adj->GetSize();
				s /=2;
				Val->SetPos(-50 +p.x + s.x, -50 + p.y+ s.y);
				MY_UI::Internal::RootWidget->MouseButtonDown(&MY_UI::Controls::cWidgetBase::LeftMouseButtonDown);// force the root widget redo a mouse down, which will give the value adjuster focus n stuff
			}

			virtual void Draw_Clip(){
				TextBox::Draw_Clip();
				
				Internal_Numeric_TextBox.Val_Adj->Draw_NoClip();
				Internal_Numeric_TextBox.Minus->Draw_NoClip();
				Internal_Numeric_TextBox.Plus->Draw_NoClip();
				if(Val) Val->Draw_Clip();
			}
			virtual void Draw_NoClip(){
				TextBox::Draw_NoClip();
				
				Internal_Numeric_TextBox.Val_Adj->Draw_NoClip();
				Internal_Numeric_TextBox.Minus->Draw_NoClip();
				Internal_Numeric_TextBox.Plus->Draw_NoClip();
				if(Val) Val->Draw_NoClip();
			}
			virtual cWidgetBase* Hit(){
				if(Val !=0 ) {
					if(Val->Hit() !=0 ) return Val;
				}
				if(Internal_Numeric_TextBox.Val_Adj->Hit() != 0) return Internal_Numeric_TextBox.Val_Adj;
				if(Internal_Numeric_TextBox.Minus->Hit() != 0) return Internal_Numeric_TextBox.Minus;
				if(Internal_Numeric_TextBox.Plus->Hit() != 0) return Internal_Numeric_TextBox.Plus;
				return TextBox::Hit();
			}
			virtual cWidgetBase* Hit_And_SetFocus(){
				if(Val !=0 ) {
					if(Val->Hit() !=0 ) return Val;
				}
				if(Internal_Numeric_TextBox.Val_Adj->Hit() != 0) return Internal_Numeric_TextBox.Val_Adj;
				if(Internal_Numeric_TextBox.Minus->Hit() != 0) return Internal_Numeric_TextBox.Minus;
				if(Internal_Numeric_TextBox.Plus->Hit() != 0) return Internal_Numeric_TextBox.Plus;
				return TextBox::Hit_And_SetFocus();
			}

		};

		template<class T>class Vector2_Adjuster : public cWidgetBase {
		public:

			Vector2_Adjuster(cWidgetBase* parent) : cWidgetBase(parent) {
				SetControlBounds(Utilities::Rect(0, 0, 300, 300));// this just needs to be big enough to hold all the boxes, afterwards, SetSizeToContents will adjust it to the correct size
				SetClientBounds(Utilities::Rect(0, 0, 300, 300));// this just needs to be big enough to hold all the boxes, afterwards, SetSizeToContents will adjust it to the correct size

				Alignment = BOTTOM;
				Box1 = new MY_UI::Controls::Numeric_TextBox_w_Adjusters<T>(this);
				Box2= new MY_UI::Controls::Numeric_TextBox_w_Adjusters<T>(this);
				Set_Box_Length(65);
				Set_Box_Alignment(Alignment);
				SetSizeToContents();
				
			}
			virtual ~Vector2_Adjuster() {}

			virtual void Set_Box_Alignment(int align){
				Alignment = align; 
				Box2->Align_To_Widget(Box1, Alignment | IN_LINE, 5);
			} 
			virtual void Set_Box_Length(int length){
				Box1->SetLength(length);
				Box2->SetLength(length);
			} 
			virtual void Set_Box_Links(T* box1, T* box2){
				Box1->SetLink(box1);
				Box2->SetLink(box2);
			}
			virtual void Set_Box_Increment(T inc){
				Box1->SetIncrement(inc);
				Box2->SetIncrement(inc);
			}
			virtual void Set_Box_Range(T min, T max){
				Box1->SetRange(min, max);
				Box2->SetRange(min, max);
			}
			MY_UI::Controls::Numeric_TextBox_w_Adjusters<T> *Box1, *Box2;
			int Alignment;
		};

		template<class T>class Vector3_Adjuster : public cWidgetBase {
		public:

			Vector3_Adjuster(cWidgetBase* parent) : cWidgetBase(parent) {
				SetControlBounds(Utilities::Rect(0, 0, 300, 300));// this just needs to be big enough to hold all the boxes, afterwards, SetSizeToContents will adjust it to the correct size
				SetClientBounds(Utilities::Rect(0, 0, 300, 300));// this just needs to be big enough to hold all the boxes, afterwards, SetSizeToContents will adjust it to the correct size

				Alignment = BOTTOM;
				Box1 = new MY_UI::Controls::Numeric_TextBox_w_Adjusters<T>(this);
				Box2= new MY_UI::Controls::Numeric_TextBox_w_Adjusters<T>(this);
				Box3 = new MY_UI::Controls::Numeric_TextBox_w_Adjusters<T>(this);
				Set_Box_Length(65);
				Set_Box_Alignment(Alignment);
				SetSizeToContents();
				
			}
			virtual ~Vector3_Adjuster() {}

			virtual void Set_Box_Alignment(int align){
				Alignment = align; 
				Box2->Align_To_Widget(Box1, Alignment | IN_LINE, 5);
				Box3->Align_To_Widget(Box2, Alignment | IN_LINE, 5);
			} 
			virtual void Set_Box_Length(int length){
				Box1->SetLength(length);
				Box2->SetLength(length);
				Box3->SetLength(length);
			} 
			virtual void Set_Box_Links(T* box1, T* box2, T* box3){
				Box1->SetLink(box1);
				Box2->SetLink(box2);
				Box3->SetLink(box3);
			}
			virtual void Set_Box_Increment(T inc){
				Box1->SetIncrement(inc);
				Box2->SetIncrement(inc);
				Box3->SetIncrement(inc);
			}
			virtual void Set_Box_Range(T min, T max){
				Box1->SetRange(min, max);
				Box2->SetRange(min, max);
				Box3->SetRange(min, max);
			}
			MY_UI::Controls::Numeric_TextBox_w_Adjusters<T> *Box1, *Box2, *Box3;
			int Alignment;
		};

		
		template<class T>class Vector4_Adjuster : public cWidgetBase {
		public:

			Vector4_Adjuster(cWidgetBase* parent) : cWidgetBase(parent) {
				SetControlBounds(Utilities::Rect(0, 0, 300, 300));// this just needs to be big enough to hold all the boxes, afterwards, SetSizeToContents will adjust it to the correct size
				SetClientBounds(Utilities::Rect(0, 0, 300, 300));// this just needs to be big enough to hold all the boxes, afterwards, SetSizeToContents will adjust it to the correct size

				Alignment = BOTTOM;
				Box1 = new MY_UI::Controls::Numeric_TextBox_w_Adjusters<T>(this);
				Box2= new MY_UI::Controls::Numeric_TextBox_w_Adjusters<T>(this);
				Box3 = new MY_UI::Controls::Numeric_TextBox_w_Adjusters<T>(this);
				Box4 = new MY_UI::Controls::Numeric_TextBox_w_Adjusters<T>(this);
				Set_Box_Length(65);
				Set_Box_Alignment(Alignment);
				SetSizeToContents();
				
			}
			virtual ~Vector4_Adjuster() {}

			virtual void Set_Box_Alignment(int align){
				Alignment = align; 
				Box2->Align_To_Widget(Box1, Alignment | IN_LINE, 5);
				Box3->Align_To_Widget(Box2, Alignment | IN_LINE, 5);
				Box4->Align_To_Widget(Box3, Alignment | IN_LINE, 5);
			} 
			virtual void Set_Box_Length(int length){
				Box1->SetLength(length);
				Box2->SetLength(length);
				Box3->SetLength(length);
				Box4->SetLength(length);
			} 
			virtual void Set_Box_Links(T* box1, T* box2, T* box3, T* box4){
				Box1->SetLink(box1);
				Box2->SetLink(box2);
				Box3->SetLink(box3);
				Box4->SetLink(box4);
			}
			virtual void Set_Box_Increment(T inc){
				Box1->SetIncrement(inc);
				Box2->SetIncrement(inc);
				Box3->SetIncrement(inc);
				Box4->SetIncrement(inc);
			}
			virtual void Set_Box_Range(T min, T max){
				Box1->SetRange(min, max);
				Box2->SetRange(min, max);
				Box3->SetRange(min, max);
				Box4->SetRange(min, max);
			}
			MY_UI::Controls::Numeric_TextBox_w_Adjusters<T> *Box1, *Box2, *Box3, *Box4;
			int Alignment;
		};
	};
};

#endif