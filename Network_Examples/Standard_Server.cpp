// Standard_Server.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "..\Network_Lib\cUDP_Server.h"
#include "..\Threading_Lib\Thread.h"
#include "..\Allocator_lib\cAllocator.h"

int main(int argc, char* argv[])
{
	Threading::Internal_Use::cThreadStarter ThreadStarter;
	NLD_Allocator::Internal::cAllocStarter AllocStarter;
	NetworkLibraryStarter netlibstarter;
	cUDP_Server s;
	if(!s.Listen(
		8676, // Listening on port 8676,
		4, // allocating slots for 4 peers. A server should allocate as many as needed. This is a one time deal
		6000, // max send bandwidth PER PEER
		6000, // max receive bandwidth PER PEER
		2)){ // number of channels PER PEER
			std::cout<<"Error starting up the Server.. exiting";
			return 0;
	}
	unsigned int t = GetTickCount();
	while(GetTickCount() - t < 15000){// run for 15 seconds
		s.Run();
	}
	system("PAUSE");
}


