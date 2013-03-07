#include "PCH.h"
#include "Utilities.h"

#ifdef _WIN32
	unsigned int MY_UI_Too::Utilities::GetTime(){ return timeGetTime(); }
#else
	unsigned int MY_UI::Utilities::GetTime(){
		struct timeval timeVal;
		gettimeofday (& timeVal, NULL);
		return timeVal.tv_sec * 1000 + timeVal.tv_usec / 1000;
	}
#endif

