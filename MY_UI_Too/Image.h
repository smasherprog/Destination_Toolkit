#ifndef MY_UI_TOO_IMAGE_H
#define MY_UI_TOO_IMAGE_H
#include "Widget.h"
#include <string>
#include "ITexture.h"

namespace MY_UI_Too{

	namespace Controls{
		class Image : public Widget{
		protected:

			MY_UI_Too::Utilities::ITexture* _Texture;
			MY_UI_Too::Utilities::UVs _UVs;

		public:
			Image(IWidget* parent);
			virtual ~Image();

			virtual void Draw() override;
			virtual bool Set_Texture(std::string file);

			virtual void Set_UVs(MY_UI_Too::Utilities::UVs uvs){_UVs=uvs; }
			virtual MY_UI_Too::Utilities::UVs Get_UVs()const { return _UVs; }
		};
	};
};


#endif