#ifndef CWIDGETSLIDER_H
#define CWIDGETSLIDER_H
#include "cWidgetBase.h"
#include "cWidgetRenderer.h"
#include "cWidgetSkin.h"
#include "Text.h"
#include "Button.h"
#include "cWidgetInternals.h"
#include "cWidgetEvents.h"
#include <sstream>

namespace MY_UI{

	namespace Controls{

		template<class T>class VerticalSlider: public  cWidgetBase {
		public:

			Internal::cInternal_Slider<T> Internal_Slider;
	
			virtual ~VerticalSlider(){}
			/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/
			
			VerticalSlider(cWidgetBase* parent): cWidgetBase(parent) { 	
				auto skin = Internal::WidgetSkin;
				Internal_Slider.Arrow = new Controls::Button(this);
				Internal_Slider.Arrow->SetDraggable(true);
				Internal_Slider.Arrow->Set_Texture(skin->Right_Slider_Arrow);
				Internal_Slider.Arrow->SetSize(static_cast<int>(skin->Right_Slider_Arrow.width ), static_cast<int>(skin->Right_Slider_Arrow.height));
				Internal_Slider.Arrow->OnLeftDrag.Add(std::bind( &MY_UI::Controls::VerticalSlider<T>::Dragging, this));
				Internal_Slider.Arrow->OnKeyUp.Add(std::bind(  &MY_UI::Controls::VerticalSlider<T>::KeyUp, this));
				Internal_Slider.Arrow->OnMouseWheelMoved.Add(std::bind(  &MY_UI::Controls::VerticalSlider<T>::MouseWheelMoved, this));

				Internal_Slider.Slider = new Controls::Button(this);
				Internal_Slider.Slider->Set_Texture(skin->Vertical_Slider);
				Internal_Slider.Slider->SetSize(static_cast<int>(skin->Vertical_Slider.width), static_cast<int>(skin->Vertical_Slider.height));
				Internal_Slider.Slider->SetPos(4, 0);
				Internal_Slider.Slider->OnKeyUp.Add(std::bind( &MY_UI::Controls::VerticalSlider<T>::KeyUp, this));
				Internal_Slider.Slider_Length= static_cast<int>(skin->Vertical_Slider.height);
				Internal_Slider.Slider->OnMouseWheelMoved.Add(std::bind(  &MY_UI::Controls::VerticalSlider<T>::MouseWheelMoved, this));
				Internal_Slider.Slider->OnLeftClicked.Add(std::bind(  &MY_UI::Controls::VerticalSlider<T>::SliderClicked, this));

				Internal_Slider.Text_Value = new Controls::Text(this);
				Internal_Slider.Text_Value->SetFontSize(20);
				Internal_Slider.Text_Value->Align_To_Widget(Internal_Slider.Slider, BOTTOM | CENTER, 5);
				Internal_Slider.Text_Value->SetText("0");
				Internal_Slider.Text_Value->SetSizeToContents();

				MY_UI::Controls::cWidgetBase::SetSize(static_cast<int>(skin->Vertical_Slider.width), static_cast<int>(skin->Vertical_Slider.height) );
				SetSize(static_cast<int>(skin->Vertical_Slider.width), static_cast<int>(skin->Vertical_Slider.height) );
				SetSliderLength(static_cast<int>(skin->Vertical_Slider.height));
				Internal_Slider.LastSetValue =1;// seed 
				SetSliderValue(0);
			}

			virtual cWidgetBase* Hit_And_SetFocus(){
				return MY_UI::Controls::cWidgetBase::Hit();
			}
			virtual void SetSliderLength(int pixels){
				Internal_Slider.Slider_Length = pixels;
				auto skin = Internal::WidgetSkin;	
				Internal_Slider.Slider->SetSize(Internal_Slider.Slider->GetSize().x, pixels);
			
				float range(static_cast<float>(pixels- Internal_Slider.Arrow->GetSize().y));
				float dif = static_cast<float>(Internal_Slider.Max - Internal_Slider.Min);
				Internal_Slider.PerPixelValue = dif/range;
				Internal_Slider.Text_Value->Align_To_Widget(Internal_Slider.Slider, BOTTOM | CENTER, 5);

				Internal_Slider.LastSetValue -=1;
				SetSliderValue(Internal_Slider.LastSetValue +1);
				SetSizeToContents();
			}
			virtual void SliderClicked(){
				// mouse is absolute position 
				float delta =static_cast<float>(New_MousePosy - Internal_Slider.Arrow->Internal_Widget.AbsolutePosition.top)/static_cast<float>(Internal_Slider.Slider->GetSize().y);// delta should be a percent of the total
				SetSliderValue(Internal_Slider.LastSetValue - static_cast<T>(( static_cast<float>((Internal_Slider.Max - Internal_Slider.Min)) *delta)));
			}
			virtual void SetSize(int x, int y){
				MY_UI::Controls::cWidgetBase::SetSize(x, y);
				Update();
			}
			virtual void KeyUp(){
				if( (Current_Key == KEY_LEFT) | (Current_Key == KEY_DOWN) | (Current_Key == KEY_SUBTRACT)){
					SetSliderValue(Internal_Slider.LastSetValue -Internal_Slider.Increment);
				} else if( (Current_Key == KEY_UP) | (Current_Key == KEY_RIGHT) | (Current_Key == KEY_ADD)){
					SetSliderValue(Internal_Slider.LastSetValue + Internal_Slider.Increment);
				}
			}
			virtual void SetRange(T min, T max) { SetMin(min); SetMax(max); }
			virtual void MouseWheelMoved(){ SetSliderValue(Internal_Slider.LastSetValue + Internal_Slider.Increment*Mouse_Wheel_Delta); }
			virtual void SetMin(T min){ Internal_Slider.Min = min; Update(); }
			virtual T GetMin(){ return Internal_Slider.Min; }
			virtual void SetMax(T max){ Internal_Slider.Max = max; Update(); }
			virtual T GetSliderValue() { return Internal_Slider.LastSetValue; }
			virtual T GetMax(){ return Internal_Slider.Max; }
			virtual void SetIncrement(T inc){ Internal_Slider.Increment = inc; Update(); }
			virtual T GetIncrement(){ return Internal_Slider.Increment; }

			virtual void SetShowing_TextValue(bool show){ Internal_Slider.Text_Value->SetHidden(show); }
			virtual bool GetShowing_TextValue(){ return Internal_Slider.Text_Value->GetHidden(); }
			virtual void SetPrecision(unsigned int decimalplaces) { Internal_Slider.Precision = decimalplaces; }
			virtual unsigned int SetPrecision() { return Internal_Slider.Precision; }
			virtual void SetSliderValue(T value){
				if(Internal_Slider.LastSetValue == value) return;// no work to do here
				Utilities::Point pos = Internal_Slider.Arrow->GetPos();
				Utilities::Point slidersize = Internal_Slider.Slider->GetSize();
				value = Utilities::Clamp<T>(value, Internal_Slider.Min, Internal_Slider.Max);
				if(value == 0) Internal_Slider.Arrow->SetPos(pos.x, slidersize.y - Internal_Slider.Arrow->GetSize().y);
				else Internal_Slider.Arrow->SetPos( pos.x, slidersize.y - static_cast<int>((value - Internal_Slider.Min)/Internal_Slider.PerPixelValue) - Internal_Slider.Arrow->GetSize().y);	
				Internal_Slider.LastSetValue = value;
				if(Internal_Slider.Data != 0) *Internal_Slider.Data = value;
				if(Internal_Slider.Text_Value->GetHidden()) return;// dont show text value
				std::ostringstream oks;
				oks.precision(Internal_Slider.Precision);
				oks<<value;
				Internal_Slider.Text_Value->SetText(oks.str());

			}
			virtual void Update(){
				if(Internal_Slider.Data != 0) return SetSliderValue(*Internal_Slider.Data); 
			}

			virtual void Dragging(){
				auto skin = Internal::WidgetSkin;
				Utilities::Point p = Internal_Slider.Arrow->GetPos();
				Utilities::Point s = Internal_Slider.Arrow->GetSize();

				if( ( p.y + s.y + New_MousePosy - Old_MousePosy > Internal_Slider.Slider->GetSize().y ) | (p.y + New_MousePosy - Old_MousePosy < 0) ) return;
				Internal_Slider.Arrow->SetPos(p.x, p.y + New_MousePosy - Old_MousePosy);
				Internal_Slider.LastSetValue = Internal_Slider.Max - static_cast<T>(Internal_Slider.PerPixelValue*(p.y + New_MousePosy - Old_MousePosy)) ;
				Internal_Slider.LastSetValue = Utilities::Clamp<T>(Internal_Slider.LastSetValue, Internal_Slider.Min, Internal_Slider.Max);
				if(Internal_Slider.Data != 0) *Internal_Slider.Data = Internal_Slider.LastSetValue;
				if(Internal_Slider.Text_Value->GetHidden()) return;// dont show text value
				std::ostringstream oks;
				oks.precision(Internal_Slider.Precision);
				oks<<Internal_Slider.LastSetValue;
				Internal_Slider.Text_Value->SetText(oks.str());
			
			}
	

			virtual void SetLink(T* ptr){
				Internal_Slider.Data = ptr;
				MY_UI::Internal::RootWidget->CheckForUpdate(this);
				Update();
			}
			virtual T* GetLink(){ return Internal_Slider.Data; }

			// this should be for when the checkbox changes. So, it is called whether it changes
			MY_UI::Internal::cWidgetCaller OnSliderChange;

		};
		template<class T>class VerticalSlider_w_Label: public cWidget_w_Label<VerticalSlider<T>>{
		public:

			VerticalSlider_w_Label(cWidgetBase* parent): cWidget_w_Label(parent) { 	}
			virtual ~VerticalSlider_w_Label(){}
	
			virtual void SetSize(int x, int y){
				cWidgetBase::SetSize( x, y);
				Widget->SetSize(x, y);
				Label->Align_To_Widget(Widget, Label->Internal_Widget.Alignment , Label->Internal_Widget.Buffer);
			}
		};
	
		template<class T>class HorizontalSlider: public  cWidgetBase {
		public:

			Internal::cInternal_Slider<T> Internal_Slider;

			
			virtual ~HorizontalSlider(){}
			/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/
			
			HorizontalSlider(cWidgetBase* parent): cWidgetBase(parent) { 	
				auto skin = Internal::WidgetSkin;

				Internal_Slider.Arrow = new Controls::Button(this);
				Internal_Slider.Arrow->SetDraggable(true);
				Internal_Slider.Arrow->Set_Texture(skin->Up_Slider_Arrow);
				Internal_Slider.Arrow->SetSize(static_cast<int>(skin->Up_Slider_Arrow.width), static_cast<int>(skin->Up_Slider_Arrow.height));
				Internal_Slider.Arrow->OnLeftDrag.Add(std::bind( &MY_UI::Controls::HorizontalSlider<T>::Dragging, this));
				Internal_Slider.Arrow->OnKeyUp.Add(std::bind(  &MY_UI::Controls::HorizontalSlider<T>::KeyUp, this));
				Internal_Slider.Arrow->OnMouseWheelMoved.Add(std::bind(  &MY_UI::Controls::HorizontalSlider<T>::MouseWheelMoved, this));

				Internal_Slider.Slider = new Controls::Button(this);
				Internal_Slider.Slider->Set_Texture(skin->Horizontal_Slider);
				Internal_Slider.Slider->SetSize(static_cast<int>(skin->Horizontal_Slider.width), static_cast<int>(skin->Horizontal_Slider.height));
				Internal_Slider.Slider->OnKeyUp.Add(std::bind( &MY_UI::Controls::HorizontalSlider<T>::KeyUp, this));
				Internal_Slider.Slider->OnMouseWheelMoved.Add(std::bind( &MY_UI::Controls::HorizontalSlider<T>::MouseWheelMoved, this));
				Internal_Slider.Slider->OnLeftClicked.Add(std::bind(  &MY_UI::Controls::HorizontalSlider<T>::SliderClicked, this));

				Internal_Slider.Text_Value = new Controls::Text(this);
				Internal_Slider.Text_Value->Align_To_Widget(Internal_Slider.Slider, RIGHT | CENTER, 5);
				Internal_Slider.Text_Value->SetFontSize(20);
				Internal_Slider.Text_Value->SetText("0");
				Internal_Slider.Text_Value->SetSizeToContents();
		
				MY_UI::Controls::cWidgetBase::SetSize(static_cast<int>(skin->Horizontal_Slider.width), static_cast<int>(skin->Horizontal_Slider.height) );
				SetSize(static_cast<int>(skin->Horizontal_Slider.width), static_cast<int>(skin->Horizontal_Slider.height) );
				SetSliderLength(static_cast<int>(skin->Horizontal_Slider.width));
				Internal_Slider.LastSetValue = 1;// seed 
				SetSliderValue(0);

			}
			virtual void SetSliderLength(int pixels){
				Internal_Slider.Slider_Length = pixels;
				auto skin = Internal::WidgetSkin;	
				Internal_Slider.Slider->SetSize(pixels, Internal_Slider.Slider->GetSize().y );
			
				float range(static_cast<float>(pixels- Internal_Slider.Arrow->GetSize().x));
				float dif = static_cast<float>(Internal_Slider.Max - Internal_Slider.Min);
				Internal_Slider.PerPixelValue = dif/range;
				Internal_Slider.Text_Value->Align_To_Widget(Internal_Slider.Slider, RIGHT | CENTER, 5);
				SetSizeToContents();
			}
			virtual void SetSize(int x, int y){	
				MY_UI::Controls::cWidgetBase::SetSize(x, y);
				Update();
			}
			virtual void SliderClicked(){
				// mouse is absolute position 
				float delta =static_cast<float>(New_MousePosx - Internal_Slider.Arrow->Internal_Widget.AbsolutePosition.left)/static_cast<float>(Internal_Slider.Slider->GetSize().x);// delta should be a percent of the total
				SetSliderValue(Internal_Slider.LastSetValue + static_cast<T>(( static_cast<float>((Internal_Slider.Max - Internal_Slider.Min)) *delta)));
			}
			virtual cWidgetBase* Hit_And_SetFocus(){
				return MY_UI::Controls::cWidgetBase::Hit();
			}
			virtual void MouseWheelMoved(){ SetSliderValue(Internal_Slider.LastSetValue + Internal_Slider.Increment*Mouse_Wheel_Delta); }
			virtual void KeyUp(){
				if( (Current_Key == KEY_LEFT) | (Current_Key == KEY_DOWN) | (Current_Key == KEY_SUBTRACT)){
					SetSliderValue(Internal_Slider.LastSetValue -Internal_Slider.Increment);
				} else if( (Current_Key == KEY_UP) | (Current_Key == KEY_RIGHT) | (Current_Key == KEY_ADD)){
					SetSliderValue(Internal_Slider.LastSetValue + Internal_Slider.Increment);
				}
			}
			virtual void SetRange(T min, T max) { SetMin(min); SetMax(max); SetSliderLength(Internal_Slider.Slider_Length); }
			virtual void SetMin(T min){ Internal_Slider.Min = min; Update(); }
			virtual T GetMin(){ return Internal_Slider.Min; }
			virtual void SetMax(T max){ Internal_Slider.Max = max; Update(); }
			virtual T GetMax(){ return Internal_Slider.Max; }
			virtual T GetSliderValue() { return Internal_Slider.LastSetValue; }
			virtual void SetIncrement(T inc){ Internal_Slider.Increment = inc; Update(); }
			virtual T GetIncrement(){ return Internal_Slider.Increment; }

			virtual void SetShowing_TextValue(bool show){ Internal_Slider.Text_Value->SetHidden(show); }
			virtual bool GetShowing_TextValue() const { return Internal_Slider.Text_Value->GetHidden(); }
			virtual void SetPrecision(unsigned int decimalplaces) { Internal_Slider.Precision = decimalplaces; }
			virtual unsigned int GetPrecision() const { return Internal_Slider.Precision; }
			virtual void SetSliderValue(T value){
				if(Internal_Slider.LastSetValue == value) return;// no work to do here
				Utilities::Point pos = Internal_Slider.Arrow->GetPos();
				value = Utilities::Clamp<T>(value, Internal_Slider.Min, Internal_Slider.Max);
				if(value == 0) Internal_Slider.Arrow->SetPos(0, pos.y);
				else Internal_Slider.Arrow->SetPos(static_cast<int>((value - Internal_Slider.Min)/Internal_Slider.PerPixelValue), pos.y );	
				Internal_Slider.LastSetValue = value;
				if(Internal_Slider.Data != 0) *Internal_Slider.Data = value;
				if(Internal_Slider.Text_Value->GetHidden()) return;// dont show text value
				std::ostringstream oks;
				oks.precision(Internal_Slider.Precision);
				oks<<value;
				Internal_Slider.Text_Value->SetText(oks.str());
			}
			virtual void Update(){
				if(Internal_Slider.Data != 0) return SetSliderValue(*Internal_Slider.Data); 
			}

			virtual void Dragging(){
				auto skin = Internal::WidgetSkin;
				Utilities::Point p = Internal_Slider.Arrow->GetPos();
				Utilities::Point s = Internal_Slider.Arrow->GetSize();

				if( ( p.x + s.x + New_MousePosx - Old_MousePosx > Internal_Slider.Slider->GetSize().x ) | (p.x + New_MousePosx - Old_MousePosx < 0) ) return;
				Internal_Slider.Arrow->SetPos(p.x + New_MousePosx - Old_MousePosx, p.y );
				Internal_Slider.LastSetValue = static_cast<T>(Internal_Slider.PerPixelValue*(p.x + New_MousePosx - Old_MousePosx) ) + Internal_Slider.Min;

				if(Internal_Slider.Data != 0) *Internal_Slider.Data = Internal_Slider.LastSetValue;
				if(Internal_Slider.Text_Value->GetHidden()) return;// dont show text value
				std::ostringstream oks;
				oks.precision(Internal_Slider.Precision);
				oks<<Internal_Slider.LastSetValue;
				Internal_Slider.Text_Value->SetText(oks.str());
			}


			virtual void SetLink(T* ptr){
				Internal_Slider.Data = ptr;
				MY_UI::Internal::RootWidget->CheckForUpdate(this);
				Update();
			}
			virtual T* GetLink(){ return Internal_Slider.Data; }

			// this should be for when the checkbox changes. So, it is called whether it changes
			MY_UI::Internal::cWidgetCaller OnSliderChange;

		};
		template<class T>class HorizontalSlider_w_Label: public cWidget_w_Label<HorizontalSlider<T>>{
		public:

			HorizontalSlider_w_Label(cWidgetBase* parent): cWidget_w_Label(parent) { 	}
			virtual ~HorizontalSlider_w_Label(){}
	
			virtual void SetSize(int x, int y){
				cWidgetBase::SetSize( x, y);
				Widget->SetSize(x, y);
				Label->Align_To_Widget(Widget, Label->Internal_Widget.Alignment , Label->Internal_Widget.Buffer);
			}


		};
	};
};

#endif

