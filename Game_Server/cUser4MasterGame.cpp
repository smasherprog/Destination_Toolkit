#include "stdafx.h"
#include "cUser4MasterGame.h"
#include "cGameServer.h"

cUser4MasterGame::cUser4MasterGame(cGameServer* game) : GameServer(*game){
}

void cUser4MasterGame::UserLoggedOut(const string& login){
	OUTPUT_DEBUG_MSG("Letting the Login Server know that the User:"<<login<<" has logged out");
	cDataPacket t(LOGINLENGTH);
	t.SetID(Network::LS_USER_LOGGEDOUT_GAMESERVER);
	memcpy(t.Data, login.c_str(), login.size()+1);
	ReliableSendTo(t);// i could use the encrypted send to, but all traffic for this should be internal and not open to outside inspection. i.e. behind a firewall!
	//ReliableEncryptedSendTo(t);
}

void cUser4MasterGame::SecureConnectionEstablished(cPeer* peer){
	OUTPUT_DEBUG_MSG("secure connection established, sending info");
	SendLoginInfo();
}
void cUser4MasterGame::ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel){
	
	switch(packet->GetID()){
		case(Network::LS_LOGIN_SUCCESS):
			fLogin_Success(packet, peer);
			break;
		case(Network::LS_ACCOUNT_LOGGED_IN):
			OUTPUT_DEBUG_MSG("Account already logged in .. ");
			break;
		case(Network::LS_BAD_ACCOUNT_INFO):
			OUTPUT_DEBUG_MSG("Bad Account info ..");
			break;
		case(Network::LS_ACCOUNT_NOT_ACTIVE):
			OUTPUT_DEBUG_MSG("Account Not Active ..");
			break;
		case(Network::LS_USER_LOGINTO_GAMESERVER):
			fUser_Loginto_GameServer(packet, peer);
			break;
	};

}

void cUser4MasterGame::SendLoginInfo(){
	cDataPacket t(LOGINLENGTH*2);
	t.SetID(Network::LS_LOGIN_REQUEST);
	char* temp = t.Data;
	memcpy(temp, Login.c_str(), Login.size() +1);// copy the null terminating char
	temp+=LOGINLENGTH;
	memcpy(temp, Password.c_str(), Password.size() +1);// copy the null terminating char
	ReliableEncryptedSendTo(t);
}

void cUser4MasterGame::fLogin_Success(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG("Login Success .. Sending a Create Server Packet");
	cDataPacket t(LOGINLENGTH+sizeof(uint16_t)*2);
	t.SetID(Network::LS_CREATE_SERVER);
	char* data = t.Data;
	memcpy(data, &ServerInfo.Name, LOGINLENGTH);
	data+=LOGINLENGTH;
	memcpy( data, &ServerInfo.Status,sizeof(uint16_t));
	data+=sizeof(uint16_t);
	memcpy( data, &ServerInfo.NumberOfUsers,sizeof(uint16_t));	
	ReliableSendTo(t);
}

void cUser4MasterGame::fUser_Loginto_GameServer(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG("Received request to fUser_Loginto_GameServer");
	char* data = packet->Data;
	size_t length = packet->Length;
	if(length < LOGINLENGTH + CryptoPP::SHA256::DIGESTSIZE) {// login, encryption key, header
		OUTPUT_DEBUG_MSG("Packet Received that was smaller than the smallest size allowed, in fUser_Loginto_GameServer");
		return;
	}
	
	data[LOGINLENGTH-1]=0;// null char
	string login = (char*)data;

	data+=LOGINLENGTH;
	GameServer.QueueUser(login, (uint8_t*)data );
}