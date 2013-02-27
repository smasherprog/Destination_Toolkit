#ifndef PCH_H
#define PCH_H


#if defined(_MSC_VER)

	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	// Windows Header Files:
	#include <windows.h>
		#pragma comment( lib, "winmm" )
	#endif

#endif