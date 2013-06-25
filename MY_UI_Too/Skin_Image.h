#ifndef MY_UI_TOO_SKIN_IMAGE_H
#define MY_UI_TOO_SKIN_IMAGE_H
#include "Widget.h"
#include <string>
#include "ITexture.h"

namespace MY_UI_Too{

	namespace Controls{
		class Skin_Image : public Widget{
		protected:
			MY_UI_Too::Utilities::UVs _UVs;
		public:
			Skin_Image(IWidget* parent=nullptr);
			virtual ~Skin_Image();

			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;

			virtual bool Set_Texture(std::string file);
			virtual bool Set_Texture(MY_UI_Too::Interfaces::ITexture* tex);
		};
	};
};


#endif