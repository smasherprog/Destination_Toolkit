#ifndef CGAMESERVER_H
#define CGAMESERVER_H
#include "cUser4MasterGame.h"
#include "../Common_Game_Objects_Lib/cUser.h"
#include <vector>
#include "../Utilities/My_Timer.h"
#include <map>
#include "../Utilities/NetLib/UDP_Engine.h"
#include "../crypto++/sha.h"
#include "cChatChannel.h"
#include <concurrent_vector.h>
#include <concurrent_queue.h>
#include "cServerTerrain.h"
#include "Server_Ocean.h"
#include "../Utilities/mutex_rw.h"

using CryptoPP::SHA256;

const string GameServerConfigFile="../Assets/GameServer.cfg";
const string PlayerNamesFile = "../Assets/PlayerNames.nam";

struct authentication {
	char key[SHA256::DIGESTSIZE];
	unsigned int waittime;
};

class cGameServer: public cBaseUDP_Server{// this server will talk to clients
public:

	std::set<std::string> PlayerNames;
	cChatChannel ChatChannels;
	cUser4MasterGame User4MasterGame;// this will talk to the login server and keep users sync'd and update the login server with info about this servers status, users, etc
	
	std::vector<cGameUser*> Users;
	std::map<std::string, authentication> WaititngAuthorization;
	Concurrency::concurrent_vector<cDataPacket*> Changes;

	std::map<uint16_t, cCharacter*> CharactersInWorld;
	Concurrency::concurrent_queue<uint16_t> CharacterIds;

	Threading::spin_mutex_rw CharacterIdsMutex, CharactersInWorldMutex;
	uint16_t LastCharacterIdsSize;

	cServerTerrain Terrain;
	size_t NumberOfPlayers, NumberOfNpcs;

	Server_Ocean Ocean;

	cGameServer();
	virtual ~cGameServer();
	void ShutDown();
	void Run();
	void RunWorld();
	//void RunChat();

	void LoadConfigFile();
	void SaveConfigFile();
	void Init();
	void DeInit();

	virtual void Connect(cPeer* peer);
	virtual void Disconnect(cPeer* peer);
	virtual void ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel);
	virtual void SecureConnectionEstablished(cPeer* peer){ OUTPUT_DEBUG_MSG("cGameServer: Secure Connection established"); cBaseUDP_Server::AuthenticatePeer(peer);}

	void QueueUser(const string& login, uint8_t* arr);

	void fLogin_Request(cDataPacket* packet, cPeer* peer);
	void fLogin_Into_World(cDataPacket* packet, cPeer* peer);
	void CreateCharacter(cDataPacket* packet, cPeer* peer);
	void MovementFunction(cDataPacket* packet, cPeer* peer, bool (cCharacter::*move)() );
	
	void InitUser(string& login, uint32_t ip, uint32_t key);
	void SendCharacters(cPeer* peer);
	void SendWorldStateTo(cPeer* peer);
	void BuildWorldUpdate(cDataPacket& packet);
	void EnterWorld(cCharacter* character, cPeer* peer);
	void ExitWorld(cCharacter* character);
	
	uint16_t GetId(Concurrency::concurrent_queue<uint16_t>& cont, Threading::spin_mutex_rw& mutex);
	void AddChange(cDataPacket* pack){ Changes.push_back(pack); }
};

const uint32_t MaxLevel=51;
extern bool GameServerRunning;


#endif


