#ifndef CD3DAPP_H
#define CD3DAPP_H

#include <string>
#include "../Graphics_Lib/cBaseD3D.h"

namespace MY_UI{
	namespace Utilities{
		class cInput;
	};
	namespace Controls{
		class Text;
	};
};
class Particle_Group;
class UI_Camera;

class D3DApp: public cBaseD3D {
public:
	D3DApp(HINSTANCE hInstance, std::string appname = "Particle Demo", unsigned int height = 768, unsigned int width = 1024);
	virtual ~D3DApp();

	void run();

	void UpdateStats();

	virtual LRESULT msgProc(unsigned int msg, WPARAM wParam, LPARAM lParam);

	My_Timer::Frame_Timer FrameTimer;

	std::string FrameStats;
	Particle_Group* particles;
	UI_Camera* UICamera;
	MY_UI::Utilities::cInput *Input;
	MY_UI::Controls::Text* Frame_Stats;
};

#endif
