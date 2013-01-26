#include "stdafx.h"
#include "UDP_Engine.h"
#include "cDataPacket.h"

void cBaseUDP_Client::I_ReceiveData(){	

	char            inBuffer[ MAXMTU ];// max size a packet could be
	int  result;
	unsigned int recvtime = Network::GetTime();
	do{
		
		result = recv( I_Socket, inBuffer, MAXMTU, 0);
		if( result == SOCKET_ERROR ){
			switch (WSAGetLastError ()) {// the following errors are common
				case WSAEWOULDBLOCK:
				case WSAECONNRESET:
					return;
			};
			OUTPUT_NETWORK_DEBUG_MSG("cBaseUDP_Client::I_ReceiveData() - recv() error "<<WSAGetLastError() );
			return;
		} else if(result < sizeof(Network::Internal::ProtocolCommandHeader)){// check for a minimum header size
			//OUTPUT_NETWORK_DEBUG_MSG("UDP_Engine::I_ReceiveData() - received a packet with less than the min required size: "<<result<<" , skipping ");// this is not an error, but a call to recv can return 0 bytes, meaning there is no data left to get
			continue;
		}	
		Network::Internal::ProtocolCommandHeader* header = reinterpret_cast<Network::Internal::ProtocolCommandHeader*>(inBuffer);// get the header
		if( header->command >= Network::Internal::NUMOFCOMMANDS) continue;// a command greater than connect settings is invalid
		if((header->ChannelID & 0x0f) > MaxChannelCount-1) continue;// user is attempting to access an invalid channel
		OUTPUT_NETWORK_DEBUG_MSG("Received a packet of the type "<<Network::Internal::CommandsToString[header->command] );
		cPeer* peer = Peers;
		if((peer->State == Network::PEER_STATE_DISCONNECTED) |
			(peer->State == Network::PEER_STATE_ACKNOWLEDGING_DISCONNECT)) continue;// if a user is disconnected skip
		if((header->command == Network::Internal::COMMAND_VERIFY_CONNECT) & (peer->State == Network::PEER_STATE_CONNECTING)){
			BytesRecieved += result;	
			++PacketsReceived;
			I_ProcessVerifyConnect(peer, inBuffer, result, recvtime );
			continue;
		}
		OUTPUT_NETWORK_DEBUG_MSG("Received a packet which is from a connected user of the type :"<<Network::Internal::CommandsToString[header->command] );
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
bool cBaseUDP_Client::I_Locking_Send(cPeer* peer, char* data, unsigned short len){
#ifdef TESTNETWORK

	int d = rand()%PACKETLOSSRANGE;// get random number [0,PACKETLOSSRANGE]
	d-=PACKETLOSSRANGE/2;// bring the delta into [-PACKETLOSSRANGE/2, PACKETLOSSRANGE/2] range
	float packetlosspercent = static_cast<float>(TARGETPACKETLOSS+d)/100.0f;// combine the base packet loss and the delta packet loss, then convert it to a decimal percent

	if(rand() < packetlosspercent * RAND_MAX) return true;// drop this packet, 

#endif
	unsigned int recvtime = Network::GetTime();
	if(recvtime - peer->NextSentTime >= 1000) {
		peer->NextSentTime = recvtime;// track each second for bandwith per peer purposes
		peer->BytesSentPerSecond=0;
	}
	if(peer->BytesSentPerSecond + len > peer->Sendbandwidth) return false;// do bandwidth check here
	peer->BytesSentPerSecond+=len;

	int  result;

	// Send the data.
#ifdef THREADING
	I_SendLock.lock();
	result = send( I_Socket, data, len, 0);
	I_SendLock.unlock();
#else
	result = send( I_Socket, data, len, 0);
#endif
	if( result < len ){
		OUTPUT_NETWORK_DEBUG_MSG("Error! cBaseUDP_Client::I_Locking_Send() - Sent "<<result<<" of "<<len<<" bytes.");
		return false;
	}
	if( result == SOCKET_ERROR ){
		OUTPUT_NETWORK_DEBUG_MSG("Error! cBaseUDP_Client::I_Locking_Send() - sendto() error "<<WSAGetLastError() );
		return false;
	}
	BytesSent+=len;
	++PacketsSent;
	return true;
}
bool cBaseUDP_Client::I_NonLocking_Send(cPeer* peer, char* data, unsigned short len){
#ifdef TESTNETWORK
	int d = rand()%PACKETLOSSRANGE;// get random number [0,PACKETLOSSRANGE]
	d-=PACKETLOSSRANGE/2;// bring the delta into [-PACKETLOSSRANGE/2, PACKETLOSSRANGE/2] range
	
	float packetlosspercent = static_cast<float>(TARGETPACKETLOSS+d)/100.0f;// combine the base packet loss and the delta packet loss, then convert it to a decimal percent

	if(rand() < packetlosspercent * RAND_MAX) return true;// drop this packet

#endif
	unsigned int recvtime = Network::GetTime();
	if(recvtime - peer->NextSentTime >= 1000) {
		peer->NextSentTime = recvtime;// track each second for bandwith per peer purposes
		peer->BytesSentPerSecond=0;
	}
	if(peer->BytesSentPerSecond + len > peer->Sendbandwidth) return false;// do bandwidth check here
	peer->BytesSentPerSecond+=len;

	int  result;

	// Send the data.
	result = send( I_Socket, data, len, 0 );
	if( result < len ){
		OUTPUT_NETWORK_DEBUG_MSG("Error! cBaseUDP_Client::I_NonLocking_Send() - Sent "<<result<<" of "<<len<<" bytes.");
		return false;
	}
	if( result == SOCKET_ERROR ){
		OUTPUT_NETWORK_DEBUG_MSG("Error! cBaseUDP_Client::I_NonLocking_Send() - sendto() error "<<WSAGetLastError() );
		return false;
	}
	BytesSent+=len;
	++PacketsSent;
	return true;
}
bool cBaseUDP_Client::ConnectTo(char* pAddress, unsigned short port, unsigned short sendbandwidth, unsigned short receivebandwidth, unsigned char numofchannels){
	UDP_Engine::clear();
	Peer = UDP_Engine::Start(pAddress, port, 0, 1, sendbandwidth, receivebandwidth, numofchannels);
	if(Peer ==NULL){
		UDP_Engine::clear();
		return false;
	} 
	return true;
}
bool cBaseUDP_Client::ConnectTo(unsigned int pAddress, unsigned short port, unsigned short sendbandwidth, unsigned short receivebandwidth, unsigned char numofchannels){
	UDP_Engine::clear();
	Peer = UDP_Engine::Start(0, port, pAddress, 1, sendbandwidth, receivebandwidth, numofchannels);
	if(Peer ==NULL){
		UDP_Engine::clear();
		return false;
	} 
	return true;
}
void cBaseUDP_Client::Run(){// this is a single threaded run for a client. The server is the parallel one
	if(!Peer) return;

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
	UDP_Engine::Run(0, 1);
}