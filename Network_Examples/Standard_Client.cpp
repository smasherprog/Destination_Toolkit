// Standard_Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\Network_Lib\cUDP_Client.h"
#include "..\Threading_Lib\Thread.h"
#include "..\Allocator_Lib\cAllocator.h"

int main(int argc, char* argv[])
{
	Threading::Internal_Use::cThreadStarter ThreadStarter;
	NLD_Allocator::Internal::cAllocStarter AllocStarter;
	NetworkLibraryStarter netlibstarter;
	cUDP_Client s;
	if(!s.ConnectTo(
		"127.0.0.1",// connect to the address "127.0.0.1", which is the local host,
		8676, // on port 8676,
		6000, // max send bandwidth
		6000, // max receive bandwidth
		2)){ // number of channels
			std::cout<<"Error starting up the Client.. exiting";
			return 0;// bad connect
	}
	// this packet will be fragmented and automatically reassembled on the receiving end
	char raellylong[] = "this is a really long string sdfsddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd\nttttttttttttttttttt \
	ttttttttttttttttttttttttttttrtttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt really long string still not long enough dfdfdfdfd\
	ddddddddddddddddddddddfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\
	i still cant believe thisdfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeewwwwwww\
	aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaghggh\
	hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh\
	rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr\
	trying to make sure this packet is fragmented fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeewwwwwww\
	aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaghggh\
	hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh\
	rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr\
	wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\
	qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqend";
	unsigned int t = GetTickCount();
	cDataPacket pack(raellylong, sizeof(raellylong));// this will fill the packet with the data
	s.ReliableSendTo(pack);
	t = GetTickCount();
	while(GetTickCount() - t < 15000){// run for 15 seconds
		s.Run();
	}
	s.DisconnectNow();// this will disconnect the peer by sending a packet to the server saying we are disconnecting, then immediately resetting the peer
	system("PAUSE");
}
