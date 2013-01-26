#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include "winsock2.h"
#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>


#if defined(_MSC_VER)
	#pragma comment(lib, "Network_Lib")
#pragma comment(lib, "Allocator_Lib")
#pragma comment(lib, "Threading_Lib")
#endif



