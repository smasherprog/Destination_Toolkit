#ifndef MY_UI_TOO_FPS_H
#define MY_UI_TOO_FPS_H
#include "Widget.h"
#include <string>
#include "ITexture.h"

namespace MY_UI_Too{

	namespace Controls{
		class Text;
		class FPS : public Widget{

			Text* _Text;
			Utilities::Timer _Timer;
		public:
			FPS(IWidget* parent=nullptr);
			virtual ~FPS(){}

			virtual void Start(){_Timer.Start();}
			virtual unsigned int Get_FPS(){ return _Timer.Get_FPS();}
			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin)override;

		};
	};
};


#endif