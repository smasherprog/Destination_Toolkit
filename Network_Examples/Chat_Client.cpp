// Chat_Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\Network_Lib\cUDP_Client.h"
#include "..\Threading_Lib\Thread.h"
#include "..\Allocator_Lib\cAllocator.h"

struct logininfo{
	char login[10];
	char pass[10];
};
const std::string randomusernames[] = {
	"jorge",
	"scott",
	"george",
	"jeffery",
	"jobobo",
	"seether",
	"jomama"
};
const std::string randommessages[] = {
	"Hello everyone",
	"Does anyone know a doctor, I have this wierd thing growing on my foot",
	"me Too!!!!",
	"No way",
	"LOL, I am tottally serious",
	"How the heck is everyone doing"
};
//This is not to be copied exactly as a general purpose chat server, but merely to show how easy it is to setup and run
class Chat_Client: public cUDP_Client{
public:
	// not all methods need to be defined. The default methods work fine

	virtual void ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel){
		
		switch(packet->GetID()){
			case(1):// chat message. I assume the chat message is formated correctly. In a real application, you should check the data to ensure it is valid and check to make sure the user is actually loged in before assuming to process the message
				return ProcessChatMessage(packet, peer);
			default:
			std::cout<<"Received a packet that was unrecognized... ignoring"<<std::endl;
		};

	}
	virtual void SecureConnectionEstablished(cPeer* peer){// This is called once the keys have been exchanged with the peer and encryption is ready to be used
		// now that a secure connection is established, I will send a my login and password!
		//now that encryption is setup, I call send the login and password. AGAIN, this is not the best way to send the login and password. I chose it because it is easy to setup		
		logininfo l;

		std::string s = randomusernames[(rand()%6)];
		memcpy(l.login, s.c_str(), s.size()+1);
		s="mypass";
		memcpy(l.pass, "mypass", 7);
		ReliableEncryptedSendTo(0, (char*)&l, sizeof(logininfo));// send the login and password with a packet id of zero
		std::cout<<"Attempting to log into the chat server with the username: "<<l.login<<std::endl;

	}

	void ProcessChatMessage(cDataPacket* packet, cPeer* peer){	
		std::string message = packet->Data;
		std::cout<<message<<std::endl;
	}
	
	void SendRandomChatMessage(){
		static unsigned int timer = GetTickCount();
		if(GetTickCount() - timer < 4000) return;// send a message every 4 seconds
		timer = GetTickCount();
		int rmsg = rand()%5;// get a random number betweeen 0 and 5
		cDataPacket pack(randommessages[rmsg].size() + 1);// message size,  and the 1 for the null terminator
		pack.SetID(1);// packet of the type one can be a chat message
		memcpy(pack.Data, randommessages[rmsg].c_str(), randommessages[rmsg].size() +1);// include the null terminator in the copy
		std::cout<<"Sending chat message of "<<randommessages[rmsg]<<std::endl;
		ReliableSendTo(pack);
	}
};

int main(int argc, char* argv[])
{
	Threading::Internal_Use::cThreadStarter ThreadStarter;
	NLD_Allocator::Internal::cAllocStarter AllocStarter;
	NetworkLibraryStarter netlibstarter;

	srand(GetTickCount());
	Chat_Client s;
	if(!s.ConnectTo(
		"127.0.0.1",// connect to the address "127.0.0.1", which is the local host,
		8676, // on port 8676,
		6000, // max send bandwidth
		6000, // max receive bandwidth
		2)){ // number of channels
			std::cout<<"Error starting up the Client.. exiting";
			return 0;// bad connect
	}
	
	s.Run();
	while(s.Peer){// run as long as the peer is connected
		s.SendRandomChatMessage();
		s.Run();
	}
	s.DisconnectNow();// this will disconnect the peer by sending a packet to the server saying we are disconnecting, then immediately resetting the peer
	return 0;
}