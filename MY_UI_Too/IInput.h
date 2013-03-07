#ifndef MY_UI_TOO_IINPUT_H
#define MY_UI_TOO_IINPUT_H
#include "IWidget.h"

// the IInput's contract is to clean up the entire library by deleting the root node, which causes everything else to be delelted
namespace MY_UI_Too{
	namespace Interfaces{
	class IInput{
	public:
		virtual ~IInput(){ SAFE_DELETE(Internal::Root_Widget);}

	};
	};
};
#endif