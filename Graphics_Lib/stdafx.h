#pragma once

#include <iostream>
#include <string>

#if defined(_MSC_VER)
	#pragma comment(lib, "d3d11")
	#pragma comment(lib, "DXGI")
	
	#if defined(_DEBUG) | defined (DEBUG)
		#pragma comment(lib, "d3dx11d")	
		#pragma comment(lib, "dxerr") // needed for dxtrace
	#else 
		#pragma comment(lib, "d3dx11")
	#endif
	#pragma comment(lib, "assimp")
	#pragma comment(lib, "MY_UI_Lib")
	#pragma comment(lib, "BV_Lib")
#endif

const std::string Asset_Dir = "..\\Assets\\";