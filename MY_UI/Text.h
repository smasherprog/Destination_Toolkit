#ifndef CWIDGETTEXT_H
#define CWIDGETTEXT_H
#include "cWidgetBase.h"
#include <string>
#include <functional>

namespace MY_UI{

	namespace Controls{
		class Text: public  cWidgetBase {
		public:
			Internal::cInternal_Text Internal_Text;

			Text(cWidgetBase* parent);
			virtual ~Text(){ }
/*-----------------------------Derived Virutal Functions go here -----------------------------------------*/
			

			virtual void SetSizeToContents();
			virtual void Draw_Clip();// this will attempt to batch draw calls for better preformance. Default is to Draw_NoClip though . . 
			virtual void Draw_NoClip();// this will draw the widget immediately. Sometimes, you need to draw stuff right now!
			virtual void SetSelectable(bool selectedable = false ){ cWidgetBase::SetSelectable(selectedable); OnLeftDoubleClick.Add(static_cast<std::function<void ()>>(std::bind(&Text::SelectAll, this)), this); }

/*-----------------------------Added Functions go here -----------------------------------------*/
			virtual void SetLink(std::string* ptr) { Internal_Text.LinkedText = ptr; MY_UI::Internal::RootWidget->CheckForUpdate(this); Internal::RootWidget->WidgetHasChanged=true; }
			virtual std::string* GetLink(){ return Internal_Text.LinkedText; }

			virtual void SetBold(bool b){Internal_Text.Bold = b; Internal::RootWidget->WidgetHasChanged=true;}
			virtual bool GetBold(bool b)const { return Internal_Text.Bold; }
			virtual void SetText(const std::string& text)	{ Internal_Text.Text = text; Internal::RootWidget->WidgetHasChanged=true; }
			virtual const std::string& GetText()			{ return Internal_Text.Text; }
			virtual void SetFontSize(float size)			{ Internal_Text.TextSize = size; Internal::RootWidget->WidgetHasChanged=true;}// sets the fontsize of the text
			virtual float GetFontSize() const				{ return Internal_Text.TextSize; }// sets the fontsize of the text
			virtual void WrapText(int xsize);
			virtual MY_UI::Utilities::Point GetTextSize();
			virtual void SelectAll();
	
		};
		
	};
};

#endif