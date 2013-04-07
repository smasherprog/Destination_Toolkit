#ifndef MY_UI_TOO_FPS_H
#define MY_UI_TOO_FPS_H
#include "Widget.h"
#include <string>
#include "ITexture.h"

namespace MY_UI_Too{

	namespace Controls{
		class Text;
		class FPS : public Widget{
			typedef std::chrono::milliseconds milliseconds;
			std::chrono::high_resolution_clock::time_point _Start;
			Text* _Text;
			long long _Last_Second;// this stores the milliseconds
			unsigned int _Current_Frame, _LastFps;
		public:
			FPS(IWidget* parent);
			virtual ~FPS(){}

			virtual void Start();
			virtual unsigned int Get_FPS();
			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin)override;

		};
	};
};


#endif