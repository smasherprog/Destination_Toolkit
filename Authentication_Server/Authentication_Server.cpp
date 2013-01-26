// test_server.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "cLoginServer.h"
#include "../Utilities/Thread.h"
#include "../Utilities/cAllocator.h"
#include "../Utilities/NetLib/UDP_Engine.H"

int main(int argc, TCHAR* argv[]){
	Threading::Internal_Use::cThreadStarter ThreadStarter;
	NLD_Allocator::Internal::cAllocStarter AllocStarter;
	NetworkLibraryStarter netlibstarter;

	cLoginServer* loginserver = new cLoginServer;
	loginserver->Run();
	delete loginserver;

	return 0;
}

