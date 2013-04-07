#ifndef MY_UI_TOO_IMAGE_H
#define MY_UI_TOO_IMAGE_H
#include "Widget.h"
#include <string>
#include "ITexture.h"

namespace MY_UI_Too{

	namespace Controls{
		class Image : public Widget{
		protected:
			MY_UI_Too::Interfaces::ITexture* _Texture;
			MY_UI_Too::Utilities::UVs _UVs;
			bool _Owns_Texture;
		public:
			Image(IWidget* parent);
			virtual ~Image();

			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;

			virtual bool Set_Texture(std::string file);
			virtual bool Set_Texture(MY_UI_Too::Interfaces::ITexture* tex, bool takeownership=true);

		};
	};
};


#endif