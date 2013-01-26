#ifndef CWIDGETWINDOW_H
#define CWIDGETWINDOW_H
#include "cWidgetBase.h"
#include <string>

namespace MY_UI{

	namespace Controls{
		
		class Image;
		class Button;
		class Text;// forward delcaration

		class Titlebar : public  cWidgetBase {
		public:
			Titlebar(cWidgetBase* parent);
			virtual ~Titlebar() {}

			virtual void Draw_Clip(); 
			virtual void Draw_NoClip(){ Draw_Clip(); }
			virtual void SetTitle(const std::string& windowtitle);
			virtual void SetSize( int x, int y);
			virtual cWidgetBase* Hit();
			virtual cWidgetBase* Hit_And_SetFocus();

			Text* Title;
			Button *Close_Button, *Minimize_Button, *Maximize_Button, *Bar;
		};
	
		class Simple_Window :public  cWidgetBase {
		public:

			Simple_Window(cWidgetBase* parent);
			virtual ~Simple_Window();

			virtual void SetSize(int x, int y);
			virtual void SetPos(int x, int y);
			virtual void SetFocus(bool focus);
			virtual void Draw_Clip(); 
			virtual void Draw_NoClip();
			virtual void Dragging();

			Controls::Image *Background;
		};


		class Window : public  cWidgetBase {
		public:

			Internal::cInternal_Window Internal_Window;

			Window(cWidgetBase* parent);
			virtual ~Window();

/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/
			virtual void SetFocus(bool focus);
			virtual void SetSize(int x, int y);
			virtual void SetPos(int x, int y);

			virtual void Draw_Clip(); 
			virtual void Draw_NoClip();


/*-----------------------------Added Functions go here -----------------------------------------*/
			virtual void SetCloseable(bool close) { Internal_Window.Closeable = close; }
			virtual bool GetCloseable()const { return Internal_Window.Closeable; }
			virtual void SetMinimizeable(bool min){ Internal_Window.Minimizeable = min; }
			virtual bool GetMinimizeable()const { return Internal_Window.Minimizeable; }
			virtual void SetMaximizeable(bool max){ Internal_Window.Maximizeable = max; }
			virtual bool GetMaximizeable()const { return Internal_Window.Maximizeable; }

			virtual void SetTitle(const std::string& windowtitle);
	
			// Close will delete the window and all its children. The SetHidden function will only HIDE the window so its not drawn, not actually destroy it
			// override any of the following functions to be blank in order to make them not work
			virtual void Close();
			virtual void Minimize();
			virtual void Maximize();
			virtual void Restore();
			virtual void Dragging();
		
			// access to the objects that make up the window. I allow this so users can add their own functions to the controls 

			Titlebar* TitleBar;
			Simple_Window* SimpleWindow;
		};



	};
};

#endif