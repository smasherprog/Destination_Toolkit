#pragma once

#include <string>

#if defined(_MSC_VER)

#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "Winmm")
#define WINDOWS_LEAN_AND_MEAN
#include <winsock2.h>
#include "Mmsystem.h"
#pragma comment(lib, "Crypto++_Lib")
#pragma comment(lib, "Allocator_Lib")
#endif


const std::string Asset_Dir = "..\\Assets\\";