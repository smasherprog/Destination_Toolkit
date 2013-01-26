#ifndef cWIDGETRESIZABLE_H
#define cWIDGETRESIZABLE_H
#include "cWidgetBase.h"

#define RESIZE_BORDER 5

namespace MY_UI{

	namespace Controls{
		class Resizable: public  cWidgetBase {
		public:

			Internal::cInternal_Resizable Internal_Resizable;

			Resizable(cWidgetBase* parent);
			virtual ~Resizable(){ }
/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/
			virtual void SetSize(int x, int y);

			virtual cWidgetBase* Hit();
			virtual cWidgetBase* Hit_And_SetFocus();

			virtual void Update() {}
/*-----------------------------Added Functions go here -----------------------------------------*/


			virtual void Resizing(cWidgetBase* widget);
			virtual void StartedResizing(cWidgetBase* widget);
			virtual void FinishedResizing(cWidgetBase* widget);

		};

		
		template<class T>class cWidget_w_Resizable : public T {
		public:
					
			Resizable* Resizable;
	
			cWidget_w_Resizable(cWidgetBase* parent): T(parent) { 

				Resizable = new MY_UI::Controls::Resizable(this);	
						
				Utilities::Rect rclient = GetClientBounds();

				Resizable->SetPos(-rclient.left - RESIZE_BORDER, -rclient.top - RESIZE_BORDER);
				
				Resizable->Internal_Resizable.TopLeft->OnLeftDrag.Add(std::bind(&MY_UI::Controls::cWidget_w_Resizable<T>::Resizing, this, std::placeholders::_1), this, true);
				Resizable->Internal_Resizable.Top->OnLeftDrag.Add(std::bind(&MY_UI::Controls::cWidget_w_Resizable<T>::Resizing, this, std::placeholders::_1), this, true);
				Resizable->Internal_Resizable.TopRight->OnLeftDrag.Add(std::bind(&MY_UI::Controls::cWidget_w_Resizable<T>::Resizing, this, std::placeholders::_1), this, true);
				Resizable->Internal_Resizable.Right->OnLeftDrag.Add(std::bind(&MY_UI::Controls::cWidget_w_Resizable<T>::Resizing, this, std::placeholders::_1), this, true);
				Resizable->Internal_Resizable.BottomRight->OnLeftDrag.Add(std::bind(&MY_UI::Controls::cWidget_w_Resizable<T>::Resizing, this, std::placeholders::_1), this, true);
				Resizable->Internal_Resizable.Bottom->OnLeftDrag.Add(std::bind(&MY_UI::Controls::cWidget_w_Resizable<T>::Resizing, this, std::placeholders::_1), this, true);
				Resizable->Internal_Resizable.BottomLeft->OnLeftDrag.Add(std::bind(&MY_UI::Controls::cWidget_w_Resizable<T>::Resizing, this, std::placeholders::_1), this, true);
				Resizable->Internal_Resizable.Left->OnLeftDrag.Add(std::bind(&MY_UI::Controls::cWidget_w_Resizable<T>::Resizing, this, std::placeholders::_1), this, true);

				Utilities::Point p = GetMinSize();
				SetMinSize(p.x, p.y);
				rclient = GetControlBounds();
				SetSize(rclient.width, rclient.height);
			
			}
			virtual ~cWidget_w_Resizable(){
				delete Resizable;
			}
			virtual void SetSize(int x, int y){	
				Resizable->SetSize(x, y);	
				Utilities::Point p = Resizable->GetSize();
				T::SetSize(p.x - RESIZE_BORDER*2, p.y - RESIZE_BORDER*2);

			}

			virtual void SetMinSize(int x, int y){
				Resizable->SetMinSize(x + RESIZE_BORDER*2, y + RESIZE_BORDER*2);
				T::SetMinSize( x, y);
			}
			virtual void Draw_Clip(){
				T::Draw_Clip();
				Resizable->Draw_Clip();
			}
			virtual void Draw_NoClip(){
				T::Draw_NoClip();
				Resizable->Draw_NoClip();
			}
			virtual void Resizing(cWidgetBase* widget){
	
				unsigned int t = widget->GetId();	
				Utilities::Point size = Resizable->GetSize();
				Utilities::Point pos = GetPos();

				int deltax = New_MousePosx - Old_MousePosx;
				int deltay = New_MousePosy - Old_MousePosy;
				switch(t){	
				case(Utilities::Tr_to_Bl):// this is top right to bottom left
					if(widget == Resizable->Internal_Resizable.BottomLeft){// this means the bottom control is being draggged!!!
						SetPos(pos.x+ deltax, pos.y);
						SetSize(size.x - deltax, size.y + deltay);
					} else {
						SetPos(pos.x, pos.y + deltay);
						SetSize(size.x + deltax, size.y - deltay);// otherwise, its the top right .. which doesnt need a new pos
					}
					break;		
				case(Utilities::U_to_D):// this is top to bottom 
					if(widget == Resizable->Internal_Resizable.Top){// this means the top control is being draggged!!!
						SetPos(pos.x, pos.y + deltay);
						SetSize(size.x, size.y - deltay);
					} else SetSize(size.x, size.y + deltay);// otherwise, its the bottom .. which doesnt need a new pos
			
					break;	
				case(Utilities::Tl_to_Br):// this is top left to bottom right
					if(widget == Resizable->Internal_Resizable.TopLeft){// this means the top left control is being draggged!!!
						SetPos(pos.x + deltax, pos.y + deltay);
						SetSize(size.x - deltax, size.y - deltay);
					} else SetSize(size.x + deltax, size.y + deltay);// otherwise, its the bottom right.. which doesnt need a new pos
			
					break;
				case(Utilities::L_to_R):// this is left to right
					if(widget == Resizable->Internal_Resizable.Left){// this means the left control is being draggged!!!
						SetPos(pos.x + deltax, pos.y);
						SetSize(size.x - deltax, size.y);
					} else SetSize(size.x + deltax, size.y);// otherwise, its the bottom .. which doesnt need a new pos
					break;
				};
			}
		};
	};
};

#endif