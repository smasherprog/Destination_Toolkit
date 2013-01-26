#include "stdafx.h"
#include "Network_DemoControl.h"
#include "../Common_Game_Objects_Lib/cBaseNetwork.h"
#include "cD3DApp.h"
#include "cChatWindow.h"
#include "cCharacterSelection.h"
#include "cChatWindow.h"
#include "Global_GameData.h"
#include "../Graphics_Lib/cMesh.h"

#define SWITCHHELPER(msg, fnc)  						\
	case(msg):										\
	fnc;										\
	break;					

std::function<void (char*, unsigned int, size_t)> Network_DemoControl::Empty_Function =[] (char*, unsigned int, size_t channel) { };// empty function

void Network_DemoControl::ConnectToGameServer(){
	cDataPacket buff(SHA256::DIGESTSIZE + LOGINLENGTH);
	buff.SetID(Network::GS_LOGIN_REQUEST);
	char *buff1=buff.Data;
	char* data= buff1;
	memcpy(data, Login.c_str(), Login.size() +1);
	data += LOGINLENGTH;
	memcpy(data, Key, SHA256::DIGESTSIZE);
	ReliableEncryptedSendTo(buff);
}

void Network_DemoControl::Prep_Function_Buffer(unsigned short int maxfunction_index){ // call this before calling register function so there are no recurring allocations
	assert(Net_Functions.size() == 0);
	Net_Functions.resize(maxfunction_index); 
}
void Network_DemoControl::Register_Function(std::function<void (char*, unsigned int, size_t channel)> func, unsigned short int index){
	assert(Net_Functions.size() > index);
	Net_Functions[index] = func;
}
void Network_DemoControl::Dispatch(unsigned short int index, char* data, unsigned int length, size_t channel){ Net_Functions[index](data, length, channel); } 
void Network_DemoControl::Init(uint32_t gameserverip, const std::string& login, const uint8_t* key){
	Login = login;// this is just the username 
	ConnectTo(gameserverip, 4001); 
	memcpy(Key, key, SHA256::DIGESTSIZE);
}
void Network_DemoControl::DeInit(){
	DisconnectNow();// this will only disconnect if we are connected
}
void Network_DemoControl::Run(){
	cBaseUDP_Client::Run();// update network
}
void Network_DemoControl::Disconnect(cPeer* peer){// if this peer disconnects, some windows must be destoryed
	MY_UI::Safe_Delete(cCharacterSelection::self);
	MY_UI::Safe_Delete(cChatWindow::self);
	cBaseUDP_Client::Disconnect(peer);
}
void Network_DemoControl::ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel){
	unsigned short int index = packet->GetID();
	Net_Functions[index](packet->Data, packet->Length, channel);

	uint16_t header= *reinterpret_cast<uint16_t*>(packet->Data);
/*
		SWITCHHELPER(Network::CHAT_PLAYER_NOT_REGISTERED, cChatWindow::self->PlayerNotRegistered(packet, peer));
		SWITCHHELPER(Network::CHAT_TOO_MANY_CHANNELS, cChatWindow::self->TooManyChannels(packet, peer));
		SWITCHHELPER(Network::CHAT_CHANNEL_ALREADY_EXISTS, cChatWindow::self->ChannelAlreadyExists(packet, peer));
		//case(Network::CHAT_CREATE_CHANNEL):
		SWITCHHELPER(Network::CHAT_CHANNEL_CREATED, cChatWindow::self->JoinedChannel(packet, peer));
		SWITCHHELPER(Network::CHAT_CHANNEL_DOESNOT_EXIST, cChatWindow::self->ChannelDoesNotExist(packet, peer));
		SWITCHHELPER(Network::CHAT_JOINED_CHANNEL, cChatWindow::self->JoinedChannel(packet, peer));
		SWITCHHELPER(Network::CHAT_ALREADY_IN_CHANNEL, cChatWindow::self->AlreadyInChannel(packet, peer));
		SWITCHHELPER(Network::CHAT_NOT_IN_CHANNEL, cChatWindow::self->NotInChannel(packet, peer));
		SWITCHHELPER(Network::CHAT_PASSWORD_INCORRECT, cChatWindow::self->PasswordIncorrect(packet, peer));
		//case(Network::CHAT_JOIN_CHANNEL):
		SWITCHHELPER(Network::CHAT_CHANNEL_MESSAGE, cChatWindow::self->ChatMessage(packet, peer));
		SWITCHHELPER(Network::CHAT_TOO_MANY_USERS, cChatWindow::self->TooManyUsers(packet, peer));	
		SWITCHHELPER(Network::CHAT_BANNED_FROM_CHANNEL, cChatWindow::self->BannedFromChannel(packet, peer));
		SWITCHHELPER(Network::CHAT_USER_JOINED_CHANNEL, cChatWindow::self->UserJoinedChannel(packet, peer));
		SWITCHHELPER(Network::CHAT_USER_LEFT_CHANNEL, cChatWindow::self->UserLeftChannel(packet, peer));
		SWITCHHELPER(Network::CHAT_SET_MODERATOR, cChatWindow::self->SetModerator(packet, peer));
		//case(Network::CHAT_ADD_TEXT_TO_CHANNEL):
		*/

	
}
void Network_DemoControl::SecureConnectionEstablished(cPeer* peer){
	OUTPUT_DEBUG_MSG("Secure connection established");
	ConnectToGameServer();
}


