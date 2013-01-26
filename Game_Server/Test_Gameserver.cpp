// Test_Gameserver.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "cGameServer.h"
#include "../Utilities/Thread.h"
#include "../Utilities/cAllocator.h"
#include "../Utilities/NetLib/UDP_Engine.H"

int main(int argc, TCHAR* argv[]){
	Threading::Internal_Use::cThreadStarter ThreadStarter;
	NLD_Allocator::Internal::cAllocStarter AllocStarter;
	NetworkLibraryStarter netlibstarter;

	cGameServer* GameServer = new cGameServer();
	GameServer->Run();
	delete GameServer;

	return 0;
}
