#ifndef CUSER4MASTERGAME_H
#define CUSER4MASTERGAME_H

#include "../Utilities/NetLib/UDP_Engine.h"
#include "../Common_Game_Objects_Lib/cBaseNetwork.h"
#include <string>

// THIS IS THE GAME SERVER talking to the login server. The two must maintain some communication to maintain snychronization
class cGameServer;

class cUser4MasterGame :public cBaseUDP_Client{
public:


	cUser4MasterGame(cGameServer* game);
	virtual ~cUser4MasterGame(){}
	
	std::string Login, Password;
	Network::cServerInfo ServerInfo;
	cGameServer& GameServer;

	virtual void ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel);
	virtual void SecureConnectionEstablished(cPeer* peer);

	void UserLoggedOut(const std::string& login);
	void SendLoginInfo();

	void fLogin_Success(cDataPacket* packet, cPeer* peer);
	void fAccount_Logged_In(cDataPacket* packet, cPeer* peer);
	void fBad_Account_Info(cDataPacket* packet, cPeer* peer);
	void fAccount_Not_Active(cDataPacket* packet, cPeer* peer);
	void fUser_Loginto_GameServer(cDataPacket* packet, cPeer* peer);

};

#endif