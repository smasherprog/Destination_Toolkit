#include "stdafx.h"

#include "Application.h"
#include "../Utilities/My_Timer.h"
#include "../Sound_Lib/SoundEngine.h"
#include "../Utilities/Thread.h"
#include "../Utilities/cAllocator.h"
#include "../Utilities/NetLib/UDP_Engine.H"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
	// create these to start up the various libraries.. their destructors shut down the libraries too
	OUTPUT_DEBUG_MSG("------------Starting the Application. Anything below this should be considered startup debug code--------")
	Threading::Internal_Use::cThreadStarter *ThreadStarter = new Threading::Internal_Use::cThreadStarter();
	NLD_Allocator::Internal::cAllocStarter *AllocStarter = new NLD_Allocator::Internal::cAllocStarter();
	NetworkLibraryStarter *netlibstarter = new NetworkLibraryStarter();
	My_Timer::RandomInit(GetTickCount());// This seeds the random number generator
	Sound_Engine::Sound_Starter soundstarter;

	Application* Renderer = new Application(hInstance);
	Renderer->run();// Main loop here
	OUTPUT_DEBUG_MSG("------------Shutting down the Application. Anything below this should be considered shutdown debug code--------")
	delete Renderer;
	delete ThreadStarter;
	delete AllocStarter;
	delete netlibstarter;

#if defined(DEBUG) || defined (_DEBUG)
	system("Pause");// to see the output from any debug before it dissapears
#endif
	return 0;
}
