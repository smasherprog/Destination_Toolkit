#include "stdafx.h"
#include "Sky_UI.h"
#include "Sky.h"
#include "../Utilities/MyMath.h"
#include "../MY_UI/Window.h"
#include "../MY_UI/Slider.h"
#include "../MY_UI/CheckBox.h"
#include "../MY_UI/DropDown.h"
#include "../MY_UI/Button.h"

void Sky_UI::Init(Sky* Sky_Demo){

	window = new MY_UI::Controls::Window(MY_UI::Internal::RootWidget);
	window->SetPos(300, 150);
	window->SetSize(300, 400);
	window->SetTitle("Sky Settings");
	window->SetCloseable(false);// dont allow the window to close

	// i will add more controls here to the shader variables so different effects can be made
}
