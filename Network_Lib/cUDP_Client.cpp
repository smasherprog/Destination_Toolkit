#include "stdafx.h"
#include "cUDP_Client.h"

std::function<void (char*, unsigned int, size_t)> cUDP_Client::Empty_Function =[] (char*, unsigned int, size_t channel) { };// empty function

void cUDP_Client::Connect(cPeer* peer){//Note: encryption CANNOT BE USED AT THIS TIME!! Encryption is only valid after a call to SecureConnectionEstablished is made, which notifies you that encryption is ready
	cBaseUDP_Client::Connect(peer);// this does not absolutly need to be here. All it does is initializes encryption with the host. If you do not want this, remove this line
	std::cout<<"Client: Peer Connected. find this line of code and replace if with any custom code you need"<<std::endl;
}
void cUDP_Client::Disconnect(cPeer* peer){
	std::cout<<"Client: Peer Disconnected. find this line of code and replace if with any custom code you need"<<std::endl;
	
	
	cBaseUDP_Client::Disconnect(peer);// always call this at the end if your disconnect function
}
void cUDP_Client::ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel){
	std::cout<<"Client: Peer ReceivePacket. find this line of code and replace if with any custom code you need. DO NOT DELETE THE PACKET WHEN DONE!!!"<<std::endl;
	//an example of how to call functions
	unsigned short int index = packet->GetID();
	Net_Functions[index](packet->Data, packet->Length, channel);
}
void cUDP_Client::SecureConnectionEstablished(cPeer* peer){// This is called once the keys have been exchanged with the peer and encryption is ready to be used
	
	std::cout<<"Client: A secure connection has been established with a peer. You can now send out encrypted information. Replace this code with your own!"<<std::endl;
	// you can now send and receive encrypted messages
}
void cUDP_Client::Prep_Function_Buffer(unsigned short int maxfunction_index){ // call this before calling register function so there are no recurring allocations and to ensure that there will be no empty slots
	assert(Net_Functions.size() == 0);
	Net_Functions.resize(maxfunction_index); 
	for(int i=0; i< maxfunction_index; i++){
		Net_Functions[i] = Empty_Function;
	}
}
void cUDP_Client::Register_Function(std::function<void (char*, unsigned int, size_t channel)> func, unsigned short int index){
	assert(Net_Functions.size() > index);
	Net_Functions[index] = func;
}