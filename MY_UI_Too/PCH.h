#ifndef PCH_H
#define PCH_H

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	// Windows Header Files:
#include <windows.h>
#include "Mmsystem.h"
	#pragma comment( lib, "winmm" )
#endif

#endif