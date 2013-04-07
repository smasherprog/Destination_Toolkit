#ifndef MY_UI_TOO_TEXT_H
#define MY_UI_TOO_TEXT_H

#include "Widget.h"

namespace MY_UI_Too{
	namespace Interfaces{
		class IFont;
	};
	namespace Controls{
		class Text: public Widget{
			Interfaces::IFont* _Font;
			unsigned int _Font_Size;
			std::string _Text;
			bool _Bold;
		public:

			Text(IWidget* widget);
			virtual ~Text() override;

			virtual void Set_Text(std::string text);
			virtual std::string Get_Text() const;

			virtual void Set_Font_Size(unsigned int size =14);
			virtual unsigned int Get_Font_Size() const;

			virtual void Set_Font(std::string fontname= "Times New Roman");
			virtual std::string Get_Font() const;

			virtual void Set_Bold(bool bold);
			virtual bool Get_Bold();

			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;

		};
	};
};

#endif