#ifndef MY_UI_TOO_IINPUT_H
#define MY_UI_TOO_IINPUT_H
#include "IWidget.h"

// the IInput's contract is to clean up the entire library by calling DeInit on the library
namespace MY_UI_Too{
	namespace Interfaces{
		class IInput{
		public:
			virtual ~IInput(){ MY_UI_Too::DeInit(); }

		};
	};
};
#endif