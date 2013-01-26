#ifndef CD3DAPP_H
#define CD3DAPP_H

#include <string>
#include "../Graphics_Lib/cBaseD3D.h"

//forward declarations
namespace MY_UI{
	namespace Utilities{
		class cInput;
	};
	namespace Controls{
		class Text;
	};
};
class Simple_Terrain;

class Application : public cBaseD3D {
public:
	Application(HINSTANCE hInstance, std::string appname = "Graphics Example", unsigned int height = 768, unsigned int width = 1024);
	virtual ~Application();

	void run();

	void UpdateStats();

	virtual LRESULT msgProc(unsigned int msg, WPARAM wParam, LPARAM lParam);

	std::string FrameStats;
	Simple_Terrain* Terrain;
	MY_UI::Utilities::cInput *Input;
	MY_UI::Controls::Text* Frame_Stats;
};

#endif
