#ifndef OCEAN_UI_H
#define OCEAN_UI_H

class Sky;

namespace MY_UI{
	namespace Controls{
		class Window;
	}

}
class Sky_UI{
public:

	void Init(Sky* Sky_Demo);
	MY_UI::Controls::Window* window;// ui window


};


#endif