#include "stdafx.h"
#include "cGameServer.h"
#include "../Common_Game_Objects_Lib/cUser.h"
#include "../Utilities/My_Timer.h"

bool GameServerRunning=false;

#define TIMEOUTBEFOREAUTH 10000 // 10 seconds
#define MAXNUMBEROFPEERS 32

cGameServer::cGameServer() : User4MasterGame(this), Ocean(this) {
	Init();
}
cGameServer::~cGameServer(){
	DeInit();
	SaveConfigFile();
}

void cGameServer::Init(){
	NumberOfPlayers=NumberOfNpcs=0;
	LastCharacterIdsSize=0;
	GameServerRunning=true;
	Terrain.Init();
	LoadConfigFile();
	Users.resize(MAXNUMBEROFPEERS);// make sure to allocate enough slots for the max number of peers this server will support
	for(size_t i(0); i<MAXNUMBEROFPEERS; i++){
		Users[i]=new cGameUser();
	}
	Listen(4001, MAXNUMBEROFPEERS); // start up the server for the clients
	User4MasterGame.ConnectTo("127.0.0.1", 4000);// start up the client that connects to the login server
	Ocean.Init();
	OUTPUT_DEBUG_MSG("Finished with Init()");
}
void cGameServer::DeInit(){
	DisconnectAll();
	User4MasterGame.DisconnectNow();
	Terrain.DeInit();
	for(size_t i(0); i<MAXNUMBEROFPEERS; i++){
		delete Users[i];
	}
	Ocean.DeInit();
	Users.clear();
}
void cGameServer::LoadConfigFile(){
	cout<<"Constructing cGameServer .. ";
	ifstream file(GameServerConfigFile.c_str());
	if(!file){
		cout<<"Could not load "<<GameServerConfigFile<<" The Server will not run correctly. Please exit"<<endl;
		return;
	}
	file>>User4MasterGame.Login>>User4MasterGame.Password>>User4MasterGame.ServerInfo.Name>>User4MasterGame.ServerInfo.Status;
	file.close();
	file.clear();
	file.open(PlayerNamesFile.c_str());
	string n;
	OUTPUT_DEBUG_MSG("Loading Player Names");
	if(file){
		while(file){
			file>>n;
			PlayerNames.insert(n);
			n.clear();
		}
		file.close();
	}
	cout<<"Finished"<<endl;

}
void cGameServer::SaveConfigFile(){
	ofstream file(GameServerConfigFile.c_str());
	if(!file){
		cout<<"Could not open "<<GameServerConfigFile<<" Unable to save server settings!!"<<endl;
		return;
	}
	file<<User4MasterGame.Login<<User4MasterGame.Password<<User4MasterGame.ServerInfo.Name<<User4MasterGame.ServerInfo.Status;
	file.close();
	file.clear();
	file.open(PlayerNamesFile.c_str());
	if(file){
		for(std::set<string>::const_iterator beg(PlayerNames.begin()); beg != PlayerNames.end(); beg++){
			file<<*beg<<endl;
		}
		file.close();
	}
}
///////////////////////////////////////////////////////////////////////////////
void cGameServer::QueueUser(const string& login, uint8_t* arr){// the login server sends a key that is used to verify the user is who he or she claims
	authentication acc;
	std::pair<std::map<string, authentication>::iterator, bool> ret = WaititngAuthorization.insert(std::map<string, authentication>::value_type(login, acc));
	if(ret.second)	{// user is not inserted yet... insert authentication info
		OUTPUT_DEBUG_MSG("Inserting User: "<<login<<" Into the waiting for authorization Queue");
		memcpy(ret.first->second.key, arr, SHA256::DIGESTSIZE);
		ret.first->second.waittime = timeGetTime();
	} 
}
void cGameServer::Connect(cPeer* peer){// this is received when a client attempts to connect
	cBaseUDP_Server::Connect(peer);
	OUTPUT_DEBUG_MSG("Connection to a client was made");
}
void cGameServer::Disconnect(cPeer* peer){
	cBaseUDP_Server::Disconnect(peer);
	if(peer->Id < Users.size()){// ensure a buffer overrun doesnt occur
		if(Users[peer->Id]->LoggedIn()){
			User4MasterGame.UserLoggedOut(Users[peer->Id]->GetLogin());// let the login server know the user is logging out
			if(Users[peer->Id]->SelectedCharacter != INVALID_CHARACTER_INDEX){
				cCharacter* c = Users[peer->Id]->Characters+ Users[peer->Id]->SelectedCharacter;
				OUTPUT_DEBUG_MSG("Removing character "<<c->Name<<", and id"<<c->Gid<<" from the chat channels and world");
				//ChatChannels.UnRegister(peer->Id);
				ExitWorld(c);
			}
			Users[peer->Id]->reset();// make sure to reset the info 
		}
	}
	OUTPUT_DEBUG_MSG("Client is Disconnecting");
}
void cGameServer::ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel){
	uint16_t header = *reinterpret_cast<uint16_t*>(packet->Data);
	switch(header){
		case(Network::GS_LOGIN_REQUEST):
			fLogin_Request(packet, peer);
			break;
		case(Network::GS_CREATE_CHARACTER):
			CreateCharacter(packet, peer);
			break;
		case(Network::GS_LOGINTO_WORLD):
			fLogin_Into_World(packet, peer);
			break;
		case(Network::GS_WALK_FORWARD):
			MovementFunction(packet, peer, &cCharacter::WalkFoward);
			break;
		case(Network::GS_STOP_MOVEMENT):
			MovementFunction(packet, peer, &cCharacter::StopMovement);
			break;
		case(Network::GS_RUN_FORWARD):
			MovementFunction(packet, peer, &cCharacter::RunFoward);
			break;	
		case(Network::GS_WALK_BACKWARD):
			MovementFunction(packet, peer, &cCharacter::WalkBackward);
			break;	
		case(Network::GS_ROTATE_LEFT):
			MovementFunction(packet, peer, &cCharacter::RotateLeft);
			break;	
		case(Network::GS_ROTATE_RIGHT):
			MovementFunction(packet, peer, &cCharacter::RotateRight);
			break;	
		case(Network::GS_STOP_ROTATION):
			MovementFunction(packet, peer, &cCharacter::StopRotation);
			break;	
		case(Network::CHAT_CREATE_CHANNEL):
			//ChatChannels.CreateChannel(packet, peer);
			break;
		case(Network::CHAT_JOIN_CHANNEL):
			//ChatChannels.JoinChannel(packet, peer);
			break;
		case(Network::CHAT_ADD_TEXT_TO_CHANNEL):
			//ChatChannels.AddTextToChannel(packet, peer);
			break;
		case(Network::GS_REQ_OCEAN_GRID):
			//Ocean.Add_Ocean_Grid(packet->, peer);
			break;
	};
}

void cGameServer::Run(){
	do{
		//Sleep(50);
		cBaseUDP_Server::Run();// always run this first to preform any disconnects, connects, or packets received
		My_Timer::Run();
		unsigned int now = timeGetTime();
		std::map<string, authentication>::iterator beg(WaititngAuthorization.begin());

		while(beg != WaititngAuthorization.end()){
			unsigned int waitime = now - beg->second.waittime;
			if( waitime > TIMEOUTBEFOREAUTH) { // 5 seconds wait time
				string log=beg->first;
				OUTPUT_DEBUG_MSG("Client has not connected within "<<waitime<<" ms, the timeout limit expired since a request was made from the login server..Removing user: "<<log<<" From the Queue");
				User4MasterGame.UserLoggedOut(log);// let the login server know the user is out
				beg = WaititngAuthorization.erase(beg);
			} else ++beg;
		}		
		User4MasterGame.Run();
		RunWorld();
		//RunChat();
	}while(GameServerRunning);
}

void cGameServer::RunWorld(){
	static float totaldt =0;
	totaldt+=My_Timer::DT;
	CharactersInWorldMutex.lock_r();

	for(std::map<uint16_t, cCharacter*>::iterator beg(CharactersInWorld.begin()); beg!= CharactersInWorld.end(); beg++ ){
		cCharacter& c = *beg->second;
		c.Run();
		c.Position.y = Terrain.GetHeight(c.Position);
	}
	CharactersInWorldMutex.unlock_r();
	if(totaldt >= 5.0f){
		cDataPacket packet;
		BuildWorldUpdate(packet);
		for(size_t i(0); i< Users.size(); i++){
			if(Users[i]->LoggedIn()){
				MultipleReliableSendTo(Users[i]->GetPeer(), packet);	
			}
		}
		totaldt=0;
	}
	if(!Changes.empty()){// if there are changes to send out.. do it
		for(size_t i(0); i< Users.size(); i++){
			if(Users[i]->LoggedIn()){
				for(size_t z(0); z< Changes.size(); z++){
					MultipleReliableSendTo(Users[i]->GetPeer(), *Changes[z]);	
				}
			}
		}
		Changes.clear();
	}
}
void cGameServer::fLogin_Request(cDataPacket* packet, cPeer* peer){// received from a user trying to connect to the game server
	char* data = packet->Data;
	size_t length = packet->Length;
	if(length < SHA256::DIGESTSIZE + LOGINLENGTH) return DisconnectPeer(peer);// disconnect the peer, user is trying sneaky stuff
	// header, key, login
	data[LOGINLENGTH-1] =0;
	string login = (char*)data;
	data+=LOGINLENGTH;
	std::map<std::string, authentication>::iterator found = WaititngAuthorization.find(login);
	if(found == WaititngAuthorization.end()){// user does not exisit . . close connection
		OUTPUT_DEBUG_MSG("User "<<login<<"is not in the Waiting For Authorization Queue. Must have timed out, or the user is trying sneaky sneaky");
		return Disconnect(peer);// disconnect the peer, user is trying sneaky stuff, or user timed out
	}
	if(memcmp(data, found->second.key, SHA256::DIGESTSIZE) !=0 ){// keys are wrong.. disconect
		OUTPUT_DEBUG_MSG("User "<<login<<" keys are wrong for the login request");
		return Disconnect(peer);
	}
	OUTPUT_DEBUG_MSG("Login Request received from "<<login<<" Is Successful");
	WaititngAuthorization.erase(found);// at this point, the user is authenticated...
	AuthenticatePeer(peer);

	Users[peer->Id]->Connect(peer);
	Users[peer->Id]->SetLogin(login);
	cDataPacket pack(0);
	pack.SetID(Network::GS_LOGIN_SUCCESS);
	ReliableSendTo(peer, pack);// let the user know the login was a success
	SendCharacters(peer);// send the characters to the account
}
void cGameServer::SendCharacters(cPeer* peer){
	OUTPUT_DEBUG_MSG("Sending a character listing to peer");
	uint32_t numofchars = 0;
	cCharacter* c = Users[peer->Id]->Characters;
	uint32_t size =1;// the 1 is for the number of characters
	for(uint32_t i(0); i< MAXCHARACTERS; i++){
		if(c[i].Created()){
			size +=c[i].DeconstructForVisualSize();
			numofchars++;
		}
	}
	cDataPacket pack(size);
	char* beg = pack.Data;
	pack.SetID(Network::GS_CHARACTER_LISTING);

	*beg++=(char)numofchars;
	for(uint8_t i(0); i< MAXCHARACTERS; i++){
		if(c[i].Created()){
			c[i].DeconstructForVisual(beg);
			OUTPUT_DEBUG_MSG("Character name "<<c[i].Name);
		}
	}
	assert(beg == pack.Data + pack.Length);
	ReliableSendTo(peer, pack);// let the user know the login was a success
}
void cGameServer::fLogin_Into_World(cDataPacket* packet, cPeer* peer){
	char* data = packet->Data;
	size_t length = packet->Length;
	if(length < 1) return DisconnectPeer(peer);// disconnect the peer, user is trying sneaky stuff
	uint8_t selected = *data;
	cDataPacket pack(0);
	pack.SetID(Network::GS_INVALID_CHARACTER);
	if(selected >= MAXCHARACTERS) return MultipleReliableSendTo(peer, pack);// invalid character, not created
	cCharacter* c = Users[peer->Id]->Characters;
	if(!c[selected].Created()) return MultipleReliableSendTo(peer, pack);// invalid character, not created
	pack.SetID(Network::GS_ALREADY_IN_WORLD);
	if(Users[peer->Id]->SelectedCharacter != INVALID_CHARACTER_INDEX) return ReliableSendTo(peer, pack);// already in world
	Users[peer->Id]->SelectedCharacter=selected;
	EnterWorld(c + selected, peer);
}
void cGameServer::CreateCharacter(cDataPacket* packet, cPeer* peer){
	cCharacter* c = Users[peer->Id]->Characters;
	uint32_t i(0);
	for(; i < MAXCHARACTERS; i++){
		if(!c[i].Created()) break;
	}
	if( i == MAXCHARACTERS){
		OUTPUT_DEBUG_MSG("Character Buffer is Full, cannot create more characters");
		return;
	}
	c[i].SetCreated();
	char* data = packet->Data;
	uint8_t size = *reinterpret_cast<uint8_t*>(data++);
	if(size >= NAMESIZE) {
		OUTPUT_DEBUG_MSG("Character name is too long, cutting off extra characters");
		size =NAMESIZE-1;
	}
	c[i].SetName(data, size);
	SendCharacters(peer);
}
void cGameServer::EnterWorld(cCharacter* character, cPeer* peer){
	
	++NumberOfPlayers;
	Users[peer->Id]->PlayerName = character->Name;
	//ChatChannels.Register(&Users[peer->Id]->PlayerName, peer->Id);
	uint16_t worldid = GetId(CharacterIds, CharacterIdsMutex);
	character->Gid = Users[peer->Id]->CharacterIdInWorld = worldid;
	cDataPacket pack(1 + 2);// selected character index, and the global id number the player will have
	char* data = pack.Data;
	pack.SetID(Network::GS_ENTER_WORLD);
	*data++ = static_cast<uint8_t>(character - Users[peer->Id]->Characters);
	*reinterpret_cast<uint16_t*>(data) = worldid;
	ReliableSendTo(peer, pack);// let the user know it is ok to log in
	SendWorldStateTo(peer);
	//insert the character after the world state is sent to prevent double sending
	CharactersInWorldMutex.lock_w();
	CharactersInWorld.insert(std::map<uint16_t, cCharacter*>::value_type(worldid, character));
	CharactersInWorldMutex.unlock_w();
	// let the world know!!!
	cDataPacket* packet = new cDataPacket(character->DeconstructForVisualSize());
	Changes.push_back(packet);
	data = packet->Data;
	packet->SetID(Network::GS_CHARACTER_ENTERED_WORLD);
	character->DeconstructForVisual(data);
	OUTPUT_DEBUG_MSG("Character "<<character->Name<<" Is Entering the world with a playerID of "<<worldid);
}
void cGameServer::ExitWorld(cCharacter* character){
	CharactersInWorldMutex.lock_w();
	if(CharactersInWorld.erase(character->Gid)){
		OUTPUT_DEBUG_MSG("Character "<<character->Name<<" Is exiting the world");
		--NumberOfPlayers;
		CharacterIds.push(character->Gid);
		CharactersInWorldMutex.unlock_w();
		// let the world know!!!
		cDataPacket* packet = new cDataPacket(sizeof(uint16_t));// just the header and the players gid
		Changes.push_back(packet);
		char* data = packet->Data;
		packet->SetID(Network::GS_CHARACTER_LEFT_WORLD);
		*reinterpret_cast<uint16_t*>(data) = character->Gid;
	}
}
void cGameServer::SendWorldStateTo(cPeer* peer){
	uint32_t size = sizeof(uint16_t);
	
	CharactersInWorldMutex.lock_r();
	uint16_t numofchars = static_cast<uint16_t>(CharactersInWorld.size());
	OUTPUT_DEBUG_MSG("Sending worldstate to peer with  "<<numofchars<< " Characters");
	for(std::map<uint16_t, cCharacter*>::iterator beg(CharactersInWorld.begin()); beg!=CharactersInWorld.end(); beg++){
		size +=beg->second->DeconstructForVisualSize();
	}

	cDataPacket pack(size);
	pack.SetID(Network::GS_WORLD_STATE);
	char* data = pack.Data;
	*reinterpret_cast<uint16_t*>(data) = numofchars;
	data+=sizeof(uint16_t);	
	for(std::map<uint16_t, cCharacter*>::iterator beg(CharactersInWorld.begin()); beg!=CharactersInWorld.end(); beg++){
		beg->second->DeconstructForVisual(data);
	}
	CharactersInWorldMutex.unlock_r();
	assert(data == pack.Data + pack.Length);
	ReliableSendTo(peer, pack);// let the user know about the other players
}
void cGameServer::BuildWorldUpdate(cDataPacket& packet){
	CharactersInWorldMutex.lock_r();
	//header, pos, turn radians, numofchars, gid
	packet.resize(sizeof(uint16_t) + (sizeof(CharacterChange)*CharactersInWorld.size()) );
	packet.SetID(Network::GS_WORLD_UPDATE);
	char* data = packet.Data;
	*reinterpret_cast<uint16_t*>(data) = CharactersInWorld.size();
	data +=2;
	for(std::map<uint16_t, cCharacter*>::iterator beg(CharactersInWorld.begin()); beg!= CharactersInWorld.end(); beg++ ){
		cCharacter& c = *beg->second;
		CharacterChange& cc = *reinterpret_cast<CharacterChange*>(data);
		cc.pos = c.Position;
		cc.look = c.Look;
		cc.gid= static_cast<uint16_t>(c.Gid);	
		cc.movementstate = c.MovementState;
		data+=sizeof(CharacterChange);
	}
	CharactersInWorldMutex.unlock_r();
	assert(data == packet.Data + packet.Length);
}
void cGameServer::MovementFunction(cDataPacket* packet, cPeer* peer, bool (cCharacter::*move)()){
	if(Users[peer->Id]->SelectedCharacter == INVALID_CHARACTER_INDEX) return DisconnectPeer(peer);// disconnect the peer, user is trying sneaky stuff
	cCharacter* character = Users[peer->Id]->Characters + Users[peer->Id]->SelectedCharacter;
	if((character->*move)()){// if a change occurs in the movement, send an update to everyone on this player
		cDataPacket* pack = new cDataPacket(sizeof(CharacterChange));
		Changes.push_back(pack);
		pack->SetID(Network::GS_CHARACTER_CHANGES);
		CharacterChange& cc = *reinterpret_cast<CharacterChange*>(pack->Data);
		cc.pos = character->Position;
		cc.look = character->Look;
		cc.gid = character->Gid;
		cc.movementstate = character->MovementState;
	}
}
uint16_t cGameServer::GetId(Concurrency::concurrent_queue<uint16_t>& cont, Threading::spin_mutex_rw& mutex){
	uint16_t id;
	if(!cont.try_pop(id)){
		mutex.lock_w();
		if(!cont.try_pop(id)){// always check afer the lock another thread might have filled the buffer
			for(uint16_t i(LastCharacterIdsSize); i< LastCharacterIdsSize + 256; i++) cont.push(i);
			LastCharacterIdsSize += 256;
			mutex.unlock_w();
			return GetId(cont, mutex);
		} else return id;// another thread filled the buffer
	}
	return id;
}
/*
class DistributeChat_p{
public:
	std::vector<cGameUser*>& users;
	DistributeChat_p(std::vector<cGameUser*>& u): users(u){}
	void operator()(const tbb::concurrent_hash_map<std::string, cChatChannel::cChannel>::const_range_type& r) const{
		for(tbb::concurrent_hash_map<std::string, cChatChannel::cChannel>::const_iterator beg(r.begin()); beg!=r.end(); beg++){// for each channel		
			const std::vector<cDataPacket>& mes = beg->second.Messages;
			for(std::map<std::string*, uint16_t>::const_iterator itr(beg->second.Users.begin()); itr!=beg->second.Users.end(); itr++){// for each user in the channel			
				for(size_t i(0); i < mes.size(); i++){// for each message in the channel send to user
					uint16_t da = *reinterpret_cast<uint16_t*>(mes[i].Data);
					OUTPUT_DEBUG_MSG("DistributeChat_p::Sending packet "<<da<<" to "<<users[itr->second]->PlayerName);
					GameServer.ReliableSendTo(users[itr->second]->GetPeer(), mes[i].Data, mes[i].Length); 
				}
			}
			if(beg->second.Name == "Default") continue;// no moderator in this channel
			uint16_t moderatorid = beg->second.ModeratorId;
			for(size_t i(0); i < mes.size(); i++){// for each message in the channel send to user
				uint16_t da = *reinterpret_cast<uint16_t*>(mes[i].Data);
				OUTPUT_DEBUG_MSG("DistributeChat_p::Sending packet "<<da<<" to the moderator "<<users[moderatorid]->PlayerName);
				GameServer.ReliableSendTo(users[moderatorid]->GetPeer(), mes[i].Data, mes[i].Length); 
			}
		}
	}
};

class DistributeTell_p{
public:
	std::vector<cGameUser*>& users;
	DistributeTell_p(std::vector<cGameUser*>& u): users(u){}
	void operator()(const tbb::concurrent_hash_map<uint16_t, cChatChannel::cUserInfo>::range_type& r) const{
		for(tbb::concurrent_hash_map<uint16_t, cChatChannel::cUserInfo>::iterator beg(r.begin()); beg!=r.end(); beg++){// for each user
			std::vector<cDataPacket>& mes = beg->second.Messages;
			for(uint32_t i(0); i!= mes.size(); i++){
				OUTPUT_DEBUG_MSG("DistributeTell_p::Sending packet to "<<users[beg->first]->PlayerName);
				GameServer.ReliableSendTo(users[beg->first]->GetPeer(), mes[i]);
			}
			mes.clear();
		}
	}
};

void cGameServer::RunChat(){
	
	DistributeTell_p dp(Users);
	tbb::parallel_for(ChatChannels.IdToName.range(64), dp, tbb::auto_partitioner());

	DistributeChat_p dp1(Users);
	tbb::parallel_for(ChatChannels.Channels.range(64), dp1, tbb::auto_partitioner());
	ChatChannels.ClearChatMessages();
}



void cGameServer::SentUserUpdate(const size_t &i){
	
	if((Users[i].LoggedIn) & (Users[i].Gid != NO_GID)){
		uint8_t active(Users[i].ActiveChar);
		uint32_t z((static_cast<uint32_t>(Users[i].Character[active].Position.z)>>BytesToShift4WContainer));
		uint32_t x((static_cast<uint32_t>(Users[i].Character[active].Position.x)>>BytesToShift4WContainer));
		SendPlayerInformation(static_cast<uint32_t>(i), z, x);
	}
	
}

void cGameServer::SendPlayerInformation(uint32_t id, uint8_t x, uint8_t z){// this is a macro update send!!
	return;// no need for this yet
	SendReliable(WContainer[z-1][x-1].GetMacroBuffer(), WContainer[z-1][x-1].MacroSize(), NULL);
	SendReliable(WContainer[z-1][x].GetMacroBuffer(), WContainer[z-1][x].MacroSize(), NULL);
	SendReliable(WContainer[z-1][x+1].GetMacroBuffer(), WContainer[z-1][x+1].MacroSize(), NULL);
	SendReliable(WContainer[z][x-1].GetMacroBuffer(), WContainer[z-1][x-1].MacroSize(), NULL);
	SendReliable(WContainer[z][x].GetMacroBuffer(), WContainer[z-1][x].MacroSize(), NULL);
	SendReliable(WContainer[z][x+1].GetMacroBuffer(), WContainer[z-1][x+1].MacroSize(), NULL);
	SendReliable(WContainer[z+1][x-1].GetMacroBuffer(), WContainer[z-1][x-1].MacroSize(), NULL);
	SendReliable(WContainer[z+1][x].GetMacroBuffer(), WContainer[z-1][x].MacroSize(), NULL);
	SendReliable(WContainer[z+1][x+1].GetMacroBuffer(), WContainer[z-1][x+1].MacroSize(), NULL);
}

void cGameServer::SendPlayerInformation(uint32_t id, uint8_t ox, uint8_t oz, uint8_t nx, uint8_t nz){
	uint8_t x(nx-ox), z(nz-oz);
	if((oz==nx) | (ox ==nx)){ // player moved up/down or left/right
		SendReliable(WContainer[nz+z][nx+x].GetMicroBuffer(), WContainer[nz+z][nx+x].MicroSize(), NULL);
		SendReliable(WContainer[nz+x+z][nx+x+z].GetMicroBuffer(), WContainer[nz+x+z][nx+x+z].MicroSize(), NULL);
		SendReliable(WContainer[nz-x+z][nx-z+x].GetMicroBuffer(), WContainer[nz-x+z][nx-z+x].MicroSize(), NULL); 
	} else { // player moved diagonally
		SendReliable(WContainer[nz+z][nx+x].GetMicroBuffer(), WContainer[nz+z][nx+x].MicroSize(), NULL);
		SendReliable(WContainer[nz][nx+x].GetMicroBuffer(), WContainer[nz][nx+x].MicroSize(), NULL);
		SendReliable(WContainer[nz+z][nx-x].GetMicroBuffer(), WContainer[nz+z][nx-x].MicroSize(), NULL); 
		SendReliable(WContainer[nz+z][nx].GetMicroBuffer(), WContainer[nz+z][nx].MicroSize(), NULL);
		SendReliable(WContainer[nz-z][nx+x].GetMicroBuffer(), WContainer[nz-z][nx+x].MicroSize(), NULL);
	}
}

*/