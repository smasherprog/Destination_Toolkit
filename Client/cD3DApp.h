#ifndef CD3DAPP_H
#define CD3DAPP_H

#include <string>
#include "../Graphics_Lib/cBaseD3D.h"
class cLoginControl;
class cDemoControl;

namespace MY_UI{
	namespace Utilities{
		class cInput;
	};
	namespace Controls{
		class Text;
	};
};


class D3DApp: public cBaseD3D {
public:
	D3DApp(HINSTANCE hInstance, std::string appname = "Destination", unsigned int height = 768, unsigned int width = 1024);
	virtual ~D3DApp();

	void run();

	void UpdateStats();

	virtual LRESULT msgProc(unsigned int msg, WPARAM wParam, LPARAM lParam);

	std::string FrameStats;
	cLoginControl* LoginControl;
	cDemoControl* DemoControl;

	MY_UI::Utilities::cInput *Input;
	MY_UI::Controls::Text* Frame_Stats;
};

#endif
