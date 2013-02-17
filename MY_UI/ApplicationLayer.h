#ifndef CWIDGET_APPLICATIONLAYER_H
#define CWIDGET_APPLICATIONLAYER_H
#include <vector>
#include "cWidgetBase.h"

// this is really the WHOLE file. All of the functionalty comes from the hit_and_setFocus going to Hit. That keeps the children from being reordered. So, checks are always done in the order in which the children were inserted
namespace MY_UI{

	namespace Controls{
		/* this ApplicationLayer class is specially designed!! as you can see, there are bells and whisltes everywhere!
			This class should be used to manage the application layer of your application to keep it seperate from the UI layer. For example, you have a camera that should capture keys and mouse input, right?
			You should also have other areas of your application that can capture keyboard or mouse input, like a terrain editor? Perhaps, you have a terrain editor that allows you to click the terrain and hold the mouse to raise it?
			where do you place that code to capture the mouse? The UI code for any application should be kept in seperate cpp files. So, derived from cWidgetBase a new class and override the keydown keyup, mouse up down, mouse mouved.. What ever you need to
			Not all of those functions need to be overridden, just for what you need.

			If a camera is set, MY_UI will always catch input because after the Interface has been checked, this Application layer is checked. First, the children of the application layer are checked and if they are not hit
			the camera is returned. 
		*/
		class ApplicationLayer: public MY_UI::Controls::cWidgetBase {
			cWidgetBase* Camera;
		public:

			ApplicationLayer() : cWidgetBase(NULL), Camera(0) { 		
				
			}// this has no parent.. it is special!!

			virtual ~ApplicationLayer(){ Safe_Delete(Camera); }

			virtual cWidgetBase* Hit(){
				for(size_t i(0); i< Internal_Widget.Children.size(); i++){
					MY_UI::Controls::cWidgetBase* temp = Internal_Widget.Children[i]->Hit();
					if(temp != 0) return temp;// return the hit child
				}
				return Camera;
			}
			virtual cWidgetBase* Hit_And_SetFocus(){
				for(size_t i(0); i< Internal_Widget.Children.size(); i++){
					MY_UI::Controls::cWidgetBase* temp = Internal_Widget.Children[i]->Hit_And_SetFocus();
					if(temp != 0) return temp;// return the hit child
				}
				return Camera;
			}

			// these should do nothing just to save a little extra cpu           
			virtual void Draw_Clip(){}
			virtual void Draw_NoClip(){}

			void SetCamera(cWidgetBase* c) { Safe_Delete(Camera); Camera =c;}
			cWidgetBase* GetCamera() { return Camera; }

		};

	};
};

#endif



