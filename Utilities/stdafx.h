#pragma once
#define THREADING 1

#if defined(_MSC_VER)
#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "winmm")	
#define WINDOWS_LEAN_AND_MEAN
#include <winsock2.h>
#include "Mmsystem.h"
#endif

#include <iostream>
#include "assert.h"
#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "MyMath.h"
#include "My_Timer.h"
// turn debug on to do memory leak checks

#if defined(DEBUG) | defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


#define DEBUG_THREADING 1
#if defined(DEBUG_THREADING)
#ifndef OUTPUT_DEBUG_MSG // nothing special about this, I use the console for standard debug info
#define OUTPUT_DEBUG_MSG(x) {												\
	std::cout<<x<<std::endl;													     	\
	}
#endif
#else
#ifndef OUTPUT_DEBUG_MSG
#define OUTPUT_DEBUG_MSG(x) {}
#endif
#endif 
