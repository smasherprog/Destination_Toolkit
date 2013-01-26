#include "stdafx.h"
#include "cLoginServer.h"
#include "../Common_Game_Objects_Lib/cInfo.h"

cLoginServer::cLoginServer(){
	Init();
}
bool cLoginServer::Init(){
	LoginServerRunning=true;
	ReBuildServerInfo();// build the initial info
	return Listen(4000, 16, UNLIMITEDBANDWIDTH, UNLIMITEDBANDWIDTH);
}
cLoginServer::~cLoginServer(){
	DisconnectAll();
}
void cLoginServer::PacketWrongSize(cPeer* peer){
	OUTPUT_DEBUG_MSG("Packet is wrong size, Disconnecting User");
	Users[peer->Id].PacketWrongSize();// increase the counter for packet wrong sized function calls, so i can track this
	return DisconnectPeer(peer);// disconnect the peer, user is trying sneaky stuff
}
void cLoginServer::Connect(cPeer* peer){
	OUTPUT_DEBUG_MSG("Client is Connecting id : "<<peer->Id);
	cBaseUDP_Server::Connect(peer);// run this each time
}

void cLoginServer::Disconnect(cPeer* peer){
	OUTPUT_DEBUG_MSG("Client is Disconnecting id : "<<peer->Id);
	cBaseUDP_Server::Disconnect(peer);// run this each time
	if(peer->Id >= Users.size()){
		OUTPUT_DEBUG_MSG("Disconnect Received that had an ID that overran the Users buffer");
		return;
	}	
	if(Users[peer->Id].GetAccountType() == Network::SERVER_ACCOUNT){// check if this is a server disconnecting		
		size_t index = Users[peer->Id].GetServerIndex();
		OUTPUT_DEBUG_MSG("Game Server, ID: "<<index<<" is Disconnecting. Removing Users ");
		if(index < GameServersInfo.size()){// make sure the server has a valid index to the servers list
			std::set<cInfo*>& users= GameServersInfo[index].Users;// easier to access this
			OUTPUT_DEBUG_MSG("Number of Users on the game server that is being disconnected is :"<<users.size());
			for(std::set<cInfo*>::iterator beg(users.begin()); beg!= users.end(); beg++){// 
				(*beg)->ServerIndex = INVALID_SERVER_INDEX;// this logs out the user from the game server
				// make sure the users can log onto other servers if their server goes down
			}
			
			GameServersInfo.erase(GameServersInfo.begin() + Users[peer->Id].GetServerIndex());
			ReBuildServerInfo();
		}
	} 
	Users[peer->Id].Disconnect(peer);
}
void cLoginServer::ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel){

	if(packet->GetID() != Network::LS_LOGIN_REQUEST){// not a login packet request packet, check to see if the user is even logged in, if not ignore everything
		if(peer->Id >= Users.size()){
			OUTPUT_DEBUG_MSG("Packet Received that had an ID that overran the Users buffer");
			return DisconnectPeer(peer);// disconnect the peer, user is trying sneaky stuff	
		} else if(!Users[peer->Id].VerifyPeers(peer) ) {// bad bad man.. accounts do not match, user is trying sneaky sneaky, disconnect
			OUTPUT_DEBUG_MSG("Packet Received that did not match the peer in the Users array");
			return DisconnectPeer(peer);// disconnect the peer, user is trying sneaky stuff	
		}// accounts match... process packet
	} //if we arrive here, it means I can saftely process the packet
	uint16_t header = *reinterpret_cast<uint16_t*>(packet->Data);
	switch(packet->GetID()){
		case(Network::LS_LOGIN_REQUEST):
			fLogin_Request(packet, peer);
			break;
		case(Network::LS_CREATE_SERVER):
			fCreate_Server(packet, peer);
			break;
		case(Network::LS_LOGINTO_GAMESERVER_REQUEST):
			fLoginto_GameServer_Request(packet, peer);
			break;
		case(Network::LS_USER_LOGGEDOUT_GAMESERVER):// this is only sent from a game server
			fUser_LoggedOut_GameServer(packet, peer);
			break;

	};

}
void cLoginServer::Run(){
	
	do{
		cBaseUDP_Server::Run();
	}while(LoginServerRunning);
}
void cLoginServer::ReBuildServerInfo(){// be carefull with byte aignment, so i make sure the data is properly copied. A simple memcpy of the entire structure might not be correct since the compiler aligns structures
	// size  = number of servers * (server name, server status, number of users) + the header of the packet + the number of servers
	size_t size = (GameServersInfo.size() * (LOGINLENGTH + sizeof(uint16_t)*2) + sizeof(uint32_t) )+ 1;
	ServerInfo.resize(size);
	size_t index =0;
	ServerInfo[index++] = (char)GameServersInfo.size();
	for(size_t i(0); i<GameServersInfo.size(); i++){

		memcpy(&ServerInfo[index], GameServersInfo[i].Name, LOGINLENGTH);
		index+=LOGINLENGTH;
		memcpy(&ServerInfo[index], &GameServersInfo[i].Status, sizeof(uint16_t));
		index+=sizeof(uint16_t);
		memcpy(&ServerInfo[index], &GameServersInfo[i].NumberOfUsers, sizeof(uint16_t));
		index+=sizeof(uint16_t);
		memcpy(&ServerInfo[index], &GameServersInfo[i].IpAddress, sizeof(uint32_t));
		index+=sizeof(uint32_t);
	}
}

void cLoginServer::fLogin_Request(cDataPacket* packet, cPeer* peer){
	if(packet->Length < LOGINLENGTH*2) return DisconnectPeer(peer);// packet wrong size, disconnect
	uint8_t* data = (uint8_t*)packet->Data;
	data[LOGINLENGTH-1]=data[LOGINLENGTH*2-1]=0;// make sure to insert null characters just in case
	std::string login(reinterpret_cast<char*>(data));
	std::string pass(reinterpret_cast<char*>(data +LOGINLENGTH));
	OUTPUT_DEBUG_MSG("User attempting to log in with the account info: " + login + " : " + pass);
	cInfo* acc = Accounts.find(login);
	if(acc!=NULL){// account was found, check password
		OUTPUT_DEBUG_MSG("Account found, checking pass, active, and login status");
		if(acc->Password != pass){// check passwords
			OUTPUT_DEBUG_MSG("Login Request Packet recieved with incorrect password");
			cDataPacket pack(0);
			pack.SetID(Network::LS_BAD_ACCOUNT_INFO);
			ReliableSendTo(peer, pack);
		} else if(acc->Active != Network::ACTIVE){
			OUTPUT_DEBUG_MSG("Login Request Packet recieved for an inactive account");
			cDataPacket pack(0);
			pack.SetID(Network::LS_ACCOUNT_NOT_ACTIVE);
			ReliableSendTo(peer, pack);
		} else if(acc->LoggedIn){
			OUTPUT_DEBUG_MSG("Login Request Packet recieved for an account already logged in");
			cDataPacket pack(0);
			pack.SetID(Network::LS_ACCOUNT_LOGGED_IN);
			ReliableSendTo(peer, pack);		
		} else {// account is ggood to go, allow login
			OUTPUT_DEBUG_MSG("Everything checks out ok, allowing login");
			AuthenticatePeer(peer);// remove the peer from this list so they wont be kicked 
			if(Users.size() <= peer->Id +1){
				OUTPUT_DEBUG_MSG("Resizing the Users array from "<<Users.size()<<" To "<<Users.size() + 16);
				Users.resize(peer->Id+16);// grow the array 
			}
			Users[peer->Id].SetInfo(acc);
			Users[peer->Id].Connect(peer);
			cDataPacket pac(0);
			pac.SetID(Network::LS_LOGIN_SUCCESS);
			ReliableSendTo(peer, pac);// let the user know the login was a success
			// and send the servers info packet which contain which servers are up n stuff
			cDataPacket packet(&ServerInfo[0], ServerInfo.size());
			packet.SetID(Network::LS_SERVER_LISTING);
			ReliableSendTo(peer, packet);
		}
	} else {
		OUTPUT_DEBUG_MSG("Login Request Packet recieved with incorrect login information");
		cDataPacket pac(0);
		pac.SetID(Network::LS_BAD_ACCOUNT_INFO);
		ReliableSendTo(peer, pac);// let the user know the login was a unsuccessfull
	}
}
void cLoginServer::fCreate_Server(cDataPacket* packet, cPeer* peer){
	if(Users[peer->Id].GetAccountType() == Network::SERVER_ACCOUNT){// Allow if the account is a server type
		
		uint8_t* data = (uint8_t*)packet->Data;
		Network::cServerInfo_S svs;
		memcpy(&svs.Name, data, LOGINLENGTH);
		data+=LOGINLENGTH;
		memcpy(&svs.Status, data, sizeof(uint16_t));
		data+=sizeof(uint16_t);
		memcpy(&svs.NumberOfUsers, data, sizeof(uint16_t));
		svs.PeerIndex = peer->Id;
		svs.IpAddress = peer->Address;
		GameServersInfo.push_back(svs);
		OUTPUT_DEBUG_MSG("Server Created, Name: "<<svs.Name);
		Users[peer->Id].SetServerIndex(static_cast<uint8_t>(GameServersInfo.size() -1));
		ReBuildServerInfo();
	} else DisconnectPeer(peer);// user is trying funny stuff, disconnect and mark account
}
void cLoginServer::fLoginto_GameServer_Request(cDataPacket* packet, cPeer* peer){// user requesting a log into the game server..
	if(packet->Length != 1) return PacketWrongSize(peer);
	size_t serv = static_cast<size_t>(*(packet->Data));
	OUTPUT_DEBUG_MSG("User id: "<<peer->Id<<" is requesting to log into a game server with in the index of: "<<serv);
	unsigned char requestedserver = *(packet->Data);
	if(serv >=GameServersInfo.size()){// server is not up
		OUTPUT_DEBUG_MSG(" Server is not up, sending login to game server faulure");
		cDataPacket buf(0);
		*reinterpret_cast<uint16_t*>(buf.Data) = Network::LS_LOGINTO_GAMESERVER_FAILURE;
		ReliableSendTo(peer, buf);
	
	}  else if(Users[peer->Id].Loginto_GameServer(requestedserver)){// login succceeess
		OUTPUT_DEBUG_MSG("Granting access to the server. Sending a list of gameservers up");
		cDataPacket buf(1);
		*reinterpret_cast<uint16_t*>(buf.Data) = Network::LS_LOGINTO_GAMESERVER_SUCCESS;
		buf.Data[0]=*(packet->Data);// send back the server index, so the client knows
		ReliableSendTo(peer, buf);
		
		// login, aeskey, header
		// send the login and the session encryption key to the game server, This way the game server can use the same encryption with the user and this will verify that the user is the correct user
		cDataPacket buffkey(LOGINLENGTH + SHA256::DIGESTSIZE );

		*reinterpret_cast<uint16_t*>(buffkey.Data) = Network::LS_USER_LOGINTO_GAMESERVER;
		char* b=buffkey.Data;
		memcpy(b, Users[peer->Id].GetLogin().c_str(), Users[peer->Id].GetLogin().size()+1);
		b+=LOGINLENGTH;
		memcpy(b, peer->EncryptionInfo.AESKey.BytePtr() , SHA256::DIGESTSIZE);
		b+=SHA256::DIGESTSIZE;
		ReliableSendTo(&Peers[GameServersInfo[requestedserver].PeerIndex], buffkey);// this is sent over internal networks, so no need to encrypt this
		GameServersInfo[serv].Users.insert(Users[peer->Id].GetInfo() );// insert the user, this is in case the game server crashes, I have a list of all to remove so accounts are not left in a crazy state

		++GameServersInfo[serv].NumberOfUsers;
		UnAuthenticatePeer(peer);
		peer->ConnectTime = GetTickCount() + TIMEOUT;// the current time, plus 500 ms. The peer will be disconnected in about a second forcibly
	} else {// login failure!
		OUTPUT_DEBUG_MSG("Peer is already logged into a server, sending login to game server faulure");
		cDataPacket buf(0);
		*reinterpret_cast<uint16_t*>(buf.Data) = Network::LS_LOGINTO_GAMESERVER_FAILURE;
		ReliableSendTo(peer, buf);
	}
}
void cLoginServer::fUser_LoggedOut_GameServer(cDataPacket* packet, cPeer* peer){// received from game servers letting the  login server know to grant access to other servers
	if(Users[peer->Id].GetAccountType() == Network::SERVER_ACCOUNT){// Allow if the account is a server type
		string login = (packet->Data);
		cInfo* acc = Accounts.find(login);
		if(acc) {
			OUTPUT_DEBUG_MSG("User: "<<login<<" has just logged out of the game server.");
			acc->ServerIndex = INVALID_SERVER_INDEX;// this logs out the user from the game server
			GameServersInfo[Users[peer->Id].GetServerIndex()].Users.erase(acc);
			--GameServersInfo[Users[peer->Id].GetServerIndex()].NumberOfUsers;

		} else OUTPUT_DEBUG_MSG("Account: "<<login<<"Not found, cannot remove user from the game server");
	} else DisconnectPeer(peer);// user is trying funny stuff, disconnect and mark account
}
