#ifndef OCEAN_UI_H
#define OCEAN_UI_H

class Ocean;
namespace MY_UI{
	namespace Controls{
		class Window;
	}

}
class UI_Ocean{
public:

	void Init(Ocean* Ocean_Demo);
	MY_UI::Controls::Window* window;// ui window
};


#endif