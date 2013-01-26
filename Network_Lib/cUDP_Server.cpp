#include "stdafx.h"
#include "cUDP_Server.h"

void cUDP_Server::Connect(cPeer* peer){
	cBaseUDP_Server::Connect(peer);// this should be called if you want to use the authentication system in place. This should be called at the beginning of this function.
	std::cout<<"Server: Peer Connected. find this line of code and replace if with any custom code you need"<<std::endl;
}
void cUDP_Server::Disconnect(cPeer* peer){
	cBaseUDP_Server::Disconnect(peer);// this should be called if you want to use the authentication system in place. This should be called at the beinning of this function.
	std::cout<<"Server: Peer Disconnected. find this line of code and replace if with any custom code you need"<<std::endl;
}
void cUDP_Server::ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel){
	std::cout<<"Server: Peer ReceivePacket. find this line of code and replace if with any custom code you need. DO NOT DELETE THE PACKET WHEN DONE!!!"<<std::endl;
	std::cout<<"Packet size is "<<packet->Length<<std::endl;
	std::string contents(packet->Data, packet->Length);
	std::cout<<"Packet contents "<<contents<<std::endl;
	// do something with the packet received!! This function can be called before a user is authenticated
	/*
	The whole purpose of a server is to authenticate users through a login and password. If you want this server to allow anyone to log in and use it, remove the calls to
	cBaseUDP_Server::Connect(peer)
	cBaseUDP_Server::Disonnect(peer)
	and cBaseUDP_Server::AuthenticatePeer(peer)

	Their whole point is to establish a list of peers not yet authenticated. So, if you want to use it, keep the code in place


	This part of the code is where you would do a login and password check, then if it is good, call authenticate
	
	cBaseUDP_Server::AuthenticatePeer(peer);// this ensures the peer is not kicked from the server because the peer has authenticated
	*/
}
void cUDP_Server::SecureConnectionEstablished(cPeer* peer){
	std::cout<<"Server: A secure connection has been established with a peer. You can now send out encrypted information. Replace this code with your own!"<<std::endl;
	
	/*
		REMOVE THIS NEXT LINE OF CODE after you set up your own method of authenticating users. I added it here to prevent users from being kicked automatically 
		for not authenticating themselves. It should be placed in the ReceivePacket function after a valid login and password is given. SecureConnectionEstablished is called once per secure connection. 
		You cannot have mutliple Secure connections. THere is one for every connect and disconnect.

	*/
	cBaseUDP_Server::AuthenticatePeer(peer);
}