#ifndef MY_UI_TOO_TEXT_BOX_H
#define MY_UI_TOO_TEXT_BOX_H
#include "Widget.h"
#include <string>
#include "ITexture.h"


namespace MY_UI_Too{

	namespace Controls{
		class Text;

		class Text_Box: public Widget{

			unsigned int _Carret_Index;
			Utilities::Timer _Timer;
		public:
			Text_Box(IWidget* parent=nullptr);
			virtual ~Text_Box(){}

			virtual void Set_Focus(bool focus) override;

			virtual void Mouse_Entered()override;

			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;

			virtual void Key_Up() override;
			//if you want to change the text, do it through this pointer.
			Text* Text;

			virtual void BackSpace();
			virtual void Tab();
			virtual void NewLine();
			virtual void Enter();
			virtual void Delete();
			virtual void Dec_Caret();
			virtual void Inc_Caret();
		};

	};
};


#endif