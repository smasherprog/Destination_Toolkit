// Chat_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\Network_Lib\cUDP_Server.h"
#include "..\Threading_Lib\Thread.h"
#include "..\Allocator_Lib\cAllocator.h"

struct logininfo{
	char login[10];
	char pass[10];
};
// this is a stupid simple chat server to merely show how authentication and chat messages can be sent
class Chat_Server: public cUDP_Server{
public:

	std::map<cPeer*, std::string> Chat_Users;
	virtual ~Chat_Server() {}

	virtual void Disconnect(cPeer* peer){
		cBaseUDP_Server::Disconnect(peer);// this should be called if you want to use the authentication system in place. This should be called at the beinning of this function.
		std::map<cPeer*, std::string>::iterator found = Chat_Users.find(peer);
		if(found == Chat_Users.end()) return;// this shouldnt happen, but it could
		std::string uname = found->second;
		std::cout<<"The user "<<uname<<" has left the chat server "<<std::endl;
		Chat_Users.erase(found);// remove the user from out list
	}
	virtual void ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel){

		// the compiler will generate a jump table once the switch becomes large enough. This is extremly fast . . .
		switch(packet->GetID()){
			case(0):// login packet
				return ProcessLoginRequest(packet, peer);
			case(1):// chat message. I assume the chat message is formated correctly. In a real application, you should check the data to ensure it is valid and check to make sure the user is actually loged in before assuming to process the message
				return ProcessChatMessage(packet, peer);
			default:
				std::cout<<"packet type was unrecognized... ignoring"<<std::endl;
		};
	}

	void ProcessLoginRequest(cDataPacket* packet, cPeer* peer){
		if(packet->Length != sizeof(logininfo)){
			std::cout<<"Login packet is not the correct length, ignoring"<<std::endl;
			return;
		}
		logininfo* temp = (logininfo*)packet->Data;// cast to this data type. 
		std::cout<<"Processing a login request. Login: "<<temp->login<<"  and Password: "<<temp->pass<<std::endl;
		// i commented out the login, so i can use that as the chat name. This is a bad idea to use in a real application, but is used in this demo for simplicity
		if( /* strcmp(temp->login, "mylogin") == 0 && */ strcmp(temp->pass, "mypass")==0){
			std::string un = temp->login;
			std::pair<std::map<cPeer*, std::string>::iterator, bool> itr = Chat_Users.insert(std::map<cPeer*, std::string>::value_type(peer, un));
			if(itr.second){
				std::cout<<"The user "<<un<<" has joined the chat server "<<std::endl;
			}
		} else {// disconnect, bad login
			DisconnectPeer(peer);
		}
	}
	void ProcessChatMessage(cDataPacket* packet, cPeer* peer){	
		// the message is assumed to be null terminating. Rememeber, this is just a sample so precautions should be taken to test message length, perhaps check it for curse words, etc
		char* data =packet->Data;// increment the data pointer passed the packettype
		std::map<cPeer*, std::string>::iterator found = Chat_Users.find(peer);// find the user
		if(found == Chat_Users.end()) return;// this shouldnt happen, but it could
		std::string uname = found->second;
		std::string chatmessage;
		chatmessage += "Chat Message: " + uname + " says, ";
		chatmessage += data;// make sure to check the data here.. very important. A user could send a message 2 Megs and crash the server or bog it down
		std::cout<<chatmessage<<std::endl;
		cDataPacket pack(chatmessage.size() + 1);// the new size of the message, plus a the null terminator on the string which is not included in the size() function
		pack.SetID(1);// packet of the type one can be a chat message
		memcpy(pack.Data, chatmessage.c_str(), chatmessage.size() +1);// include the null terminator in the copy
		for(std::map<cPeer*, std::string>::iterator beg(Chat_Users.begin()); beg!= Chat_Users.end(); beg++){// send the chat message to all connected peers
			cPeer* p = beg->first;
			MultipleReliableSendTo(p, pack);// use the multiple call which will copy the packet and not comsume it on a sendto call
		}
	}
};
int main(int argc, char* argv[])
{
	Threading::Internal_Use::cThreadStarter ThreadStarter;
	NLD_Allocator::Internal::cAllocStarter AllocStarter;
	NetworkLibraryStarter netlibstarter;
	Chat_Server s;
	if(!s.Listen(
		8676, // Listening on port 8676,
		4, // allocating slots for 4 peers. A server should allocate as many as needed. This is a one time deal
		6000, // max send bandwidth PER PEER
		6000, // max receive bandwidth PER PEER
		2)){ // number of channels PER PEER
			std::cout<<"Error starting up the Server.. exiting";
			return 0;
	}
	while(1){// run for ever!!
		s.Run();
	}

}
