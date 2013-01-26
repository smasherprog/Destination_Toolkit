#pragma once

#include <string>
#include <iostream>
#include <sstream>


#if defined(_MSC_VER)
	#pragma comment(lib, "Graphics_Lib")
	#pragma comment(lib, "MY_UI_Lib")
	#pragma comment(lib, "MY_UI_DX_Renderer_Lib")
	#pragma comment(lib, "assimp")
	#pragma comment(lib, "BV_Lib")
#endif

const std::string Asset_Dir = "..\\Assets\\";