// Client.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "cD3DApp.h"
#include "../Sound_Lib/SoundEngine.h"
#include "../Utilities/Thread.h"
#include "../Utilities/cAllocator.h"
#include "../Utilities/NetLib/UDP_Engine.H"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
	Threading::Internal_Use::cThreadStarter ThreadStarter;
	NLD_Allocator::Internal::cAllocStarter AllocStarter;
	NetworkLibraryStarter netlibstarter;
	My_Timer::RandomInit(GetTickCount());// This seeds the random number generator
	Sound_Engine::Sound_Starter soundstarter;
	
	D3DApp* Renderer = new D3DApp(hInstance);
	Renderer->run();// run
	delete Renderer;
	Sound_Engine::DeInit();
	return 0;
}
