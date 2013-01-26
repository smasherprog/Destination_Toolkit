// Client.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "cD3DApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {

	D3DApp* Renderer = new D3DApp(hInstance);
	Renderer->run();// run
	delete Renderer;
	return 0;
}
