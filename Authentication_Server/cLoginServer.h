#ifndef CLOGINSERVER_H
#define CLOGINSERVER_H

#include "cAccounts.h"
#include "../Common_Game_Objects_Lib/cUser.h"
#include "..\Utilities\NetLib\UDP_Engine.h"
#include <set>

class cLoginServer: public cBaseUDP_Server {
	cLoginServer& operator =(const cLoginServer& rhs);// make sure this doesnt happen
public:

	bool LoginServerRunning;
	cAccounts Accounts;
	std::vector<cUser> Users;
	std::vector<Network::cServerInfo_S> GameServersInfo;
	std::vector<char> ServerInfo;

	cLoginServer();
	virtual ~cLoginServer();

	bool Init();
	void Run();
	void ReBuildServerInfo();

	virtual void Connect(cPeer* peer);
	virtual void Disconnect(cPeer* peer);
	virtual void ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel);
	virtual void SecureConnectionEstablished(cPeer* peer){ OUTPUT_DEBUG_MSG("cLoginServer: Secure Connection established"); cBaseUDP_Server::AuthenticatePeer(peer); }

	void PacketWrongSize(cPeer* peer);
	void fLogin_Request(cDataPacket* packet, cPeer* peer);
	void fCreate_Server(cDataPacket* packet, cPeer* peer);
	void fLoginto_GameServer_Request(cDataPacket* packet, cPeer* peer);
	void fUser_LoggedOut_GameServer(cDataPacket* packet, cPeer* peer);
		
};


#endif