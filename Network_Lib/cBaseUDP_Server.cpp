#include "stdafx.h"
#include "UDP_Engine.h"
#include "cDataPacket.h"
#include "../Threading_Lib/Thread.h"

#define AUTHENTICATIONCHECKTIMEOUT 10000 // if a user does not authenticate himself after 10 seconds disconnect
#define AUTHENTICATIONCHECK 200 //it is low priority, check every 1/5 of a second

void	cBaseUDP_Server::I_ReceiveData(){

	char            inBuffer[ MAXMTU ];// max size a packet could be
	SOCKADDR_IN     fromAddr;
	int             fromLen=sizeof( SOCKADDR );
	int  result;
	unsigned int recvtime = Network::GetTime();

	do{
		
		result = recvfrom( I_Socket, inBuffer, MAXMTU, 0, (SOCKADDR *)&fromAddr, &fromLen );
		if( result == SOCKET_ERROR ){
			switch (WSAGetLastError ()) {// the following errors are common
				case WSAEWOULDBLOCK:
				case WSAECONNRESET:
					return;
			};
			OUTPUT_NETWORK_DEBUG_MSG("UDP_Engine::I_ReceiveData() - recvfrom() error "<<WSAGetLastError() );
			return;
		} else if(result < sizeof(Network::Internal::ProtocolCommandHeader)){// check for a minimum header size
			//OUTPUT_NETWORK_DEBUG_MSG("UDP_Engine::I_ReceiveData() - received a packet with less than the min required size: "<<result<<" , skipping ");// this is not an error, but a call to recv can return 0 bytes, meaning there is no data left to get
			continue;
		}	
		Network::Internal::ProtocolCommandHeader* header = reinterpret_cast<Network::Internal::ProtocolCommandHeader*>(inBuffer);// get the header
		if( header->command >= Network::Internal::NUMOFCOMMANDS) continue;// a command greater than connect settings is invalid
		if((header->ChannelID & 0x0f) > MaxChannelCount-1) continue;// user is attempting to access an invalid channel
		unsigned char sessionid = (header->PeerID & 0xe000)>>13;// the session id is stored in the first three bits. This just adds a little extra help for different connections
		unsigned short peerid = (header->PeerID & ~0xe000);
		OUTPUT_NETWORK_DEBUG_MSG("Received a packet of the type "<<Network::Internal::CommandsToString[header->command] );
		if(header->command == Network::Internal::COMMAND_CONNECT){// user attempting to connect
			BytesRecieved += result;		
			++PacketsReceived;
			I_ReceiveConnect(inBuffer, result, fromAddr.sin_addr.s_addr , fromAddr.sin_port, recvtime );
			continue;
		} else if(peerid > MaxNumOfPeers) continue;// overflow.. peerid is invalid
		cPeer* peer = Peers + peerid;
		if((peer->State == Network::PEER_STATE_DISCONNECTED) | 
			(peer->State == Network::PEER_STATE_ACKNOWLEDGING_DISCONNECT)) continue;// if a user is disconnected skip

		if((!peer->I_VerifyInfo(fromAddr.sin_addr.s_addr, fromAddr.sin_port, sessionid)) | (peer->State != Network::PEER_STATE_CONNECTED)) continue;// bad info
		OUTPUT_NETWORK_DEBUG_MSG("Received a packet which is from a logged in user of the type :"<<Network::Internal::CommandsToString[header->command] );
		BytesRecieved += result;
		++PacketsReceived;
		switch(header->command){
			case(Network::Internal::COMMAND_ACKNOWLEDGE):
			case(Network::Internal::COMMAND_DISCONNECT):
			case(Network::Internal::COMMAND_UNRELIABLE_FRAGMENT):
			case(Network::Internal::COMMAND_ENCRYPTED_UNRELIABLE_PACKET):		
			case(Network::Internal::COMMAND_UNRELIABLE):
				if(recvtime - peer->NextReceiveTime >= 1000) {
					peer->NextReceiveTime = recvtime;// track each second 
					peer->BytesReceivedPerSecond=0;
				}
				peer->BytesReceivedPerSecond+=result;
				peer->I_ProcessIncomingUnReliable(header->ChannelID & 0x0f, inBuffer, result, recvtime, header->SequenceNumber);
				peer->NextTimeout = recvtime + TIMEOUT;
				break;			
			case(Network::Internal::COMMAND_PING):
			case(Network::Internal::COMMAND_RELIABLE_FRAGMENT):
			case(Network::Internal::COMMAND_INIT_ENCRYPTION):
			case(Network::Internal::COMMAND_ENCRYPTED_RELIABLE_PACKET):			
			case(Network::Internal::COMMAND_RELIABLE):
				if(recvtime - peer->NextReceiveTime >= 1000) {
					peer->NextReceiveTime = recvtime;// track each second 
					peer->BytesReceivedPerSecond=0;
				}
				peer->BytesReceivedPerSecond+=result;
				peer->I_ProcessIncomingReliable(header->ChannelID & 0x0f, inBuffer, result, recvtime, header->SequenceNumber);
				peer->NextTimeout = recvtime + TIMEOUT;
				break;

			default:// bad packet.. 
				continue;
		};
	} while( (result != 0));// once  result ==0, all the data has been read... we are done
}

bool cBaseUDP_Server::I_Locking_Send(cPeer* peer, char* data, unsigned short len){
#ifdef TESTNETWORK
	int d = rand()%PACKETLOSSRANGE;// get random number [0,PACKETLOSSRANGE]
	d-=PACKETLOSSRANGE/2;// bring the delta into [-PACKETLOSSRANGE/2, PACKETLOSSRANGE/2] range
	
	float packetlosspercent = static_cast<float>(TARGETPACKETLOSS+d)/100.0f;// combine the base packet loss and the delta packet loss, then convert it to a decimal percent
	int r = rand();
	int packetloss = static_cast<int>(packetlosspercent * RAND_MAX);// expand to [0, RAND_MAX]
	if(r < packetloss) return true;// drop this packet, 
#endif
	unsigned int recvtime = Network::GetTime();
	if(recvtime - peer->NextSentTime >= 1000) {
		peer->NextSentTime = recvtime;// track each second 
		peer->BytesSentPerSecond=0;
	}
	if(peer->BytesSentPerSecond + len > peer->Sendbandwidth) return false;// do bandwidth check here
	peer->BytesSentPerSecond+=len;
	
	SOCKADDR_IN     remoteAddr;
	unsigned short  result;
	memset( &remoteAddr, 0, sizeof( SOCKADDR_IN ) );
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_addr.s_addr = peer->Address;
	remoteAddr.sin_port = peer->DestPort;
	// Send the data.
#ifdef THREADING
	I_SendLock.lock();// you must ensure mutual access to send, otherwise, data can be interleaved from multiple threads calling send
	result = sendto( I_Socket, data, len, 0, (SOCKADDR *)&remoteAddr, sizeof( SOCKADDR ) );
	I_SendLock.unlock();
#else
	result = sendto( I_Socket, data, len, 0, (SOCKADDR *)&remoteAddr, sizeof( SOCKADDR ) );
#endif
	if( result < len ){
		OUTPUT_NETWORK_DEBUG_MSG("Error! cBaseUDP_Server::I_Locking_Send() - Sent "<<result<<" of "<<len<<" bytes.");
		return false;
	}
	if( result == SOCKET_ERROR ){
		OUTPUT_NETWORK_DEBUG_MSG("Error! cBaseUDP_Server::I_Locking_Send() - sendto() error "<<WSAGetLastError() );
		return false;
	}
	BytesSent+=len;
	++PacketsSent;
	return true;
}
bool cBaseUDP_Server::I_NonLocking_Send(cPeer* peer, char* data, unsigned short len){
#ifdef TESTNETWORK
	int d = rand()%PACKETLOSSRANGE;// get random number [0,PACKETLOSSRANGE]
	d-=PACKETLOSSRANGE/2;// bring the delta into [-PACKETLOSSRANGE/2, PACKETLOSSRANGE/2] range
	
	float packetlosspercent = static_cast<float>(TARGETPACKETLOSS+d)/100.0f;// combine the base packet loss and the delta packet loss, then convert it to a decimal percent
	int r = rand();
	int packetloss = static_cast<int>(packetlosspercent * RAND_MAX);// expand to [0, RAND_MAX]
	if(r < packetloss) return true;// drop this packet, 
#endif
	unsigned int recvtime = Network::GetTime();
	if(recvtime - peer->NextSentTime >= 1000) {
		peer->NextSentTime = recvtime;// track each second 
		peer->BytesSentPerSecond=0;
	}
	if(peer->BytesSentPerSecond + len > peer->Sendbandwidth) return false;// do bandwidth check here
	peer->BytesSentPerSecond+=len;
	SOCKADDR_IN     remoteAddr;
	unsigned short  result;
	memset( &remoteAddr, 0, sizeof( SOCKADDR_IN ) );
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_addr.s_addr = peer->Address;
	remoteAddr.sin_port = peer->DestPort;
	// Send the data.
	result = sendto( I_Socket, data, len, 0, (SOCKADDR *)&remoteAddr, sizeof( SOCKADDR ) );
	if( result < len ){
		OUTPUT_NETWORK_DEBUG_MSG("Error! cBaseUDP_Server::I_NonLocking_Send() - Sent "<<result<<" of "<<len<<" bytes.");
		return false;
	}
	if( result == SOCKET_ERROR ){
		OUTPUT_NETWORK_DEBUG_MSG("Error! cBaseUDP_Server::I_NonLocking_Send() - sendto() error "<<WSAGetLastError() );
		return false;
	}
	BytesSent+=len;
	++PacketsSent;
	return true;
}
bool cBaseUDP_Server::Listen(unsigned short port, unsigned short numofpeers, unsigned short sendbandwidth, unsigned short receivebandwidth, unsigned char numofchannels){
	UDP_Engine::Start(0, port, 0, numofpeers, sendbandwidth, receivebandwidth, numofchannels);
	if(!UDP_Engine::ServerRunning()){
		UDP_Engine::clear();
		return false;
	} 
	return true;
}

void cBaseUDP_Server::DisconnectAll(){
	for(cPeer* peer(Peers); peer!=Peers + NumOfPeers; ++peer){// for each peeer
		DisconnectPeerNow(peer);
	}
}

void cBaseUDP_Server::Run(){

	I_ReceiveData();
#ifdef TESTNETWORK
	static unsigned int lastsendtime = 0;
	static int d = 0;
	if(d==0){// calculate a new ping wait period
		d = rand()%PINGRANGE;// get random number [0,PINGRANGE]
		d -=PINGRANGE/2;// bring the delta into [-PINGRANGE/2, PINGRANGE/2] range
		d = (TARGETPING*2)+d;// combine the base ping and the delta ping

	}

	if(Network::GetTime() - lastsendtime < d) return; // if enough time has not passed, then skip this run
	lastsendtime = Network::GetTime();
	d=0;// calculate a new ping wait interval
#endif
	Threading::Parallel_For(0, MaxNumOfPeers, 48, [this] (unsigned int beg, unsigned int end) { UDP_Engine::Run(beg, end); });

	static unsigned int LastAutenticationCheck =Network::GetTime();

	if(Network::GetTime() - LastAutenticationCheck > AUTHENTICATIONCHECK){// do not check the un authenticated users repeatedly each loop, just check in every so often
		unsigned int time = LastAutenticationCheck= Network::GetTime();
		std::set<cPeer*>::iterator beg(I_UnAuthenticatedPeers_.begin());
		while(beg!= I_UnAuthenticatedPeers_.end()){
			if( time - (*beg)->ConnectTime > AUTHENTICATIONCHECKTIMEOUT){// disconnect the user, he has not authenticated himself. I dont want people hanging around using up resources
				cPeer* p = *beg;// need a copy
				beg = I_UnAuthenticatedPeers_.erase(beg);// this will move us to the next peer
				DisconnectPeerNow(p);// the peer has not authenticated himself, disconnect. A copy of the pointer, instead of *beg was passed because the disconnectpeernow calls the virtual disconnect(cPeer* peer) function so it would screw up my loop by removing the user in it			
			} else ++beg;
		}
	}
}
void cBaseUDP_Server::AuthenticatePeer(cPeer* peer){
	I_UnAuthenticatedPeers_.erase(peer);// insert this peer into the set. If the user does not authenticate themself within the time of AUTHENTICATIONCHECKTIMEOUT, Disconnect the peer
	//InitializeEncryption should not go here. The client should generally be the one to request this because the server should react to requests, not initiate requests.
	OUTPUT_NETWORK_DEBUG_MSG("Server: Peer is authenticated. ");
}
void cBaseUDP_Server::UnAuthenticatePeer(cPeer* peer){// this can be used to remove peers
	I_UnAuthenticatedPeers_.insert(peer);
}