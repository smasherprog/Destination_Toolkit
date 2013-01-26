#pragma once
//YOU MUST DEFINE IN THIS ORDER OTHERWISE the headers get all screwed up
		#define WINDOWS_LEAN_AND_MEAN
		#include "winsock2.h"
		#include <windows.h>
// END screwed up header ordering!!

#include <string>
#include <iostream>
#include <sstream>


#if defined(_MSC_VER)

#pragma comment(lib, "Sound_Lib")
#pragma comment(lib, "Graphics_Lib")
	#pragma comment(lib, "Common_Game_Objects_Lib")

#if defined(USE_WOODY3D)
#pragma comment(lib, "Woody_Lib")
#endif
#endif

const std::string Asset_Dir = "..\\Assets\\";