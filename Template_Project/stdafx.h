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
#pragma comment(lib, "Graphics_Lib")
#pragma comment(lib, "MY_UI_Lib")
#pragma comment(lib, "MY_UI_DX_Renderer_Lib")

#endif

const std::string Asset_Dir = "..\\Assets\\";