#ifndef CUSER4LOGIN_H
#define CUSER4LOGIN_H

#include "../Common_Game_Objects_Lib/cUser.h"
#include "../Common_Game_Objects_Lib/cCharacter.h"
#include "../Common_Game_Objects_Lib/cInfo.h"
#include "../Common_Game_Objects_Lib/cBaseNetwork.h"
#include "../Utilities/Utilities.h"

class cPeer;
// this is really a wrapper for the cInfo pointer and other stuff i decide to stuff in here
class cUser{
public:

	cUser(): Info(new cInfo) { Old_Info = Info; }
	~cUser() { delete Old_Info; }// old_info is always good to delete. Info is just used to hold the pointer
	void SetInfo(cInfo* acc) { Old_Info = Info; Info = acc; }
	cInfo* GetInfo() const { return Info; }

	const	string& GetLogin() const { return Info->Login; }
	void	SetLogin(const string& log) { Info->Login = log; }

	void	SetServerIndex(const uint8_t server){ Info->ServerIndex=server; }
	uint8_t GetServerIndex() const { return Info->ServerIndex; }

	void	SetAccountType(const uint8_t ty){ Info->Type=ty; }
	uint8_t GetAccountType() const { return Info->Type; }

	void	CalledwoPermission() { ++(Info->CalledwoPermission); }
	void	PacketWrongSize(){ ++(Info->PacketsNotSize); }

	bool	VerifyPeers(const cPeer* peer) const { return Info->Peer == peer;}
	cPeer*	GetPeer() const { return Info->Peer; }
	void	reset(){ Info = Old_Info; Info->clear(); }// resets this structure and all parts to factory settings! I will now install AOL
	
	bool	LoggedIn() const { return Info->LoggedIn; }
	bool	Loginto_GameServer(const uint8_t serverindex){ 	
		if(Info->ServerIndex == INVALID_SERVER_INDEX) {
			Info->ServerIndex = serverindex;
			return true;
		} else return false;
	}
	void	Logoutof_GameServer(){ Info->ServerIndex = INVALID_SERVER_INDEX; }
	bool	Connect(cPeer* peer){
		if(Info->LoggedIn) return false;
		Info->Peer= peer;
		Info->LoggedIn = true;
		return true;
	}
	bool	Disconnect(cPeer* peer){
		if(!VerifyPeers(peer)) return false;// bad bad man.. accounts do not match
		OUTPUT_DEBUG_MSG("Disconnecting User: "<<Info->Login<<"    From the server");
		Info->LoggedIn = false;
		Info->Peer =0;
		return true;
	}


protected:
	cInfo *Info, *Old_Info;
};

class cGameUser: public cUser{
public:

	cGameUser(): SelectedCharacter(INVALID_CHARACTER_INDEX){ }
	~cGameUser() {}
	
	void reset(){ cUser::reset(); SelectedCharacter = INVALID_CHARACTER_INDEX; CharacterIdInWorld=0; PlayerName.clear();}
	uint16_t SelectedCharacter, CharacterIdInWorld;
	std::string PlayerName;
	cCharacter Characters[MAXCHARACTERS];

};

#endif
