#include "stdafx.h"
#include "UDP_Engine.h"
#include "cDataPacket.h"
#include <list>


bool UDP_Engine::I_Networkstarted(false);
bool UDP_Engine::I_NetworkRunning(){
	if(!I_Networkstarted) {
		OUTPUT_NETWORK_DEBUG_MSG("Network is not running. Invalid call to a network function");
		return false;
	}
	return true;
}
void UDP_Engine::clear(){
	if(I_Socket != 0) closesocket( I_Socket );
	I_Socket=0;
	delete [] Peers;
	Peers =0;
	PacketsSent=PacketsReceived=0;
	PacketsLost=BytesRecieved = BytesSent =0;
	NumOfPeers=0;
	MaxChannelCount=MAXCHANNELCOUNT;
	MaxMTU = DEFAULTMTU;
	I_Client=I_Server=false;
	PerPeerReceiveBandwidth = MINPEERBANDWIDTH;
	PerPeerSendBandwidth = MINPEERBANDWIDTH;
	MaxNumOfPeers =  MINPEERCOUNT;
	MaxChannelCount = MINCHANNELCOUNT;
	I_PeerIds.clear();
	I_ConnectedIPs.clear();
}
bool UDP_Engine::Initialize_Network(){
	OUTPUT_NETWORK_DEBUG_MSG( "UDP_Engine::Initialize_Network() - Initializing Winsock." );
#ifdef WIN32
	timeBeginPeriod(1);
	srand(GetTickCount());// seed the randomizer! This is used for networking packet loss and ping simulations. The randomness isn't of a huge concern.
	WSAData wsaData;
	int     error = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	if( error == SOCKET_ERROR ){
		error = WSAGetLastError();
		if( error == WSAVERNOTSUPPORTED ){
			OUTPUT_NETWORK_DEBUG_MSG("UDP_Engine::Initialize_Network() - WSAStartup() error.\nRequested v2.2, found only v,"<<LOBYTE( wsaData.wVersion )<<HIBYTE( wsaData.wVersion ) );
			WSACleanup();
		} else OUTPUT_NETWORK_DEBUG_MSG("UDP_Engine::Initialize_Network() - WSAStartup() error "<<WSAGetLastError() );
		I_Networkstarted=false;// just in case
		return false;
	}
#endif
	I_Networkstarted=true;
	return true;
}
void UDP_Engine::ShutDown_Network(){ 
#ifdef WIN32	
	if(I_Networkstarted) WSACleanup(); 
	timeEndPeriod(1);
#endif 
}
bool UDP_Engine::I_Pre_Startup(unsigned short peercount, unsigned short sendbandwidth, unsigned short receivebandwidth, unsigned char numofchannels){
	if(!I_NetworkRunning() ) return NULL;
	if(I_Socket !=0 || I_Client || I_Server ){
		OUTPUT_NETWORK_DEBUG_MSG("Host already doing network activity. Create additional hosts. One host per I_Client, One host per server");
		return false;
	}
	clear();// this will reset everything!!!
	PerPeerSendBandwidth = sendbandwidth = clamp<unsigned short>(sendbandwidth, MINPEERBANDWIDTH, UNLIMITEDBANDWIDTH);
	PerPeerReceiveBandwidth= receivebandwidth = clamp<unsigned short>(receivebandwidth, MINPEERBANDWIDTH, UNLIMITEDBANDWIDTH);	
	MaxChannelCount = numofchannels = clamp<unsigned char>(numofchannels, MINCHANNELCOUNT, MAXCHANNELCOUNT);
	MaxNumOfPeers = clamp<unsigned short>(peercount, MINPEERCOUNT, MAXPEERCOUNT);
	Peers= new cPeer[MaxNumOfPeers];
	for(unsigned short i(0); i< MaxNumOfPeers; i++){
		Peers[i].ChannelCount = MaxChannelCount;
		Peers[i].I_Channels = new cChannel[MaxChannelCount];// allocate channels
		I_PeerIds.insert(i);// insert ids
	}	
	OUTPUT_NETWORK_DEBUG_MSG("UDP_Engine:: - Creating the send socket");
	I_Socket = socket( AF_INET, SOCK_DGRAM, 0 );
	if( I_Socket == INVALID_SOCKET ){
		OUTPUT_NETWORK_DEBUG_MSG("UDP_Engine:: - socket() error "<< WSAGetLastError() );
		clear();
		return false;
	}
	return true;
}
cPeer* UDP_Engine::Start(char* pAddress, unsigned short port, unsigned int alternative_addr, unsigned short peercount, unsigned short sendbandwidth, unsigned short receivebandwidth, unsigned char numofchannels){
	if(port ==0 || !I_Pre_Startup(peercount, sendbandwidth, receivebandwidth, numofchannels)) return NULL;
	if(pAddress!=0 && alternative_addr !=0) return NULL;// cannot specifiy two different addresses, either a string or an unsigned int
	/* 
	A little explaining about the bind below. Bind does not imply a Destination, its for the Source. For example, if you call Bind with an address, it does not mean the packets are bound to that address
	, it means the packets think they are being sent from that address. The same follows for a port. If you specify a port on a Bind, that is the port that packets will be RECEIVED from. So, if you bind port 900, 
	that means packets are received on that port. The sending port is assigned by the OS and is completly random, but is fixed until you call closesocket( ) or WSAcleanup(). Binding port 0, tells the OS to bind to a random listening port
	A client only cares about where the packets are being send to. binding a specific listening port doesnt make any sense because the server will receive that information when it receives a packet from the client. So, the server will know how to respond.
	*/
	port=htons(port);// convert to network byte order
	SOCKADDR_IN localAddr;
	memset( &localAddr, 0, sizeof( SOCKADDR_IN ) );
	localAddr.sin_family = AF_INET;	
	localAddr.sin_port = port;// a client will connect to the port, a server will listen, either way, its the same code
	int result;
	if(pAddress){// CLIENT code
		assert(alternative_addr==0);
		hostent *hostEntry = gethostbyname (pAddress);
		if (hostEntry == NULL || hostEntry -> h_addrtype != AF_INET) {
			unsigned long host = inet_addr (pAddress);
			if (host == INADDR_NONE){
			   clear();
			   return false;
			}
			localAddr.sin_addr.s_addr = host;
		} else {
			localAddr.sin_addr.s_addr = *(unsigned int*)hostEntry -> h_addr_list[0]; // a client connects to an address
		}
		result = connect(I_Socket, (sockaddr *)&localAddr, sizeof( SOCKADDR_IN ) );
	} else if(alternative_addr== 0){// SERVER CODE
		assert(pAddress==0);
		localAddr.sin_addr.s_addr =  INADDR_ANY ;// a server does not connect to an address, but accepts any address
		result = bind( I_Socket, (sockaddr *)&localAddr, sizeof( SOCKADDR_IN ) );
	}else {// a number must be specified as an address!
		assert(alternative_addr!=0);
		localAddr.sin_addr.s_addr = alternative_addr;//alternative_addr should be good to go. The caller must ensure that this is a valid address, otherwise .. . BOOM!
		result = connect(I_Socket, (sockaddr *)&localAddr, sizeof( SOCKADDR_IN ) );
	}

	
	if( result == SOCKET_ERROR ){
		closesocket( I_Socket );
		OUTPUT_NETWORK_DEBUG_MSG("UDP_Engine:: - Network error "<< WSAGetLastError() );
		clear();
		return NULL;
	}
	unsigned long int val= 1;  // Anything non-zero.
	result = ioctlsocket( I_Socket, FIONBIO, &val );// set up a non blocking socket
	if( result ){
		closesocket( I_Socket );
		OUTPUT_NETWORK_DEBUG_MSG("UDP_Engine:: - ioctlsocket() error "<<WSAGetLastError() );
		clear();
		return NULL;
	}
	unsigned int addr =alternative_addr;// if this is a client specifying a string as an address, this should be zero, and if not, it should be valid
	if(pAddress) addr = inet_addr(pAddress);// make sure its a client

	if(pAddress == NULL) {// this is a server... 
		OUTPUT_NETWORK_DEBUG_MSG("Starting up a server . . . Successfull");
		 I_Server = true;
		return NULL;
	} else {// if an address is specifiied, that means this host will be set up as a client.
		I_Client = true;
		static unsigned char sessionid(0);// this is just a random number that helps differentiate mutliple connection attempts from the same ip
		if(++sessionid > MAXSESSIONID){
			sessionid=0;
		}

		
		unsigned char sessid = sessionid;		
		unsigned short id = 0;// this will always be zero for now

		OUTPUT_NETWORK_DEBUG_MSG("Client Created . . . Sending Connect request");
		Peers[id].I_SetupConnect(addr, port, id, sessid, DEFAULTMTU, receivebandwidth, sendbandwidth, numofchannels);// set up stufff
		Internal_DataPacket* packet( new Internal_DataPacket( sizeof(Network::Internal::ProtocolConnect) ) );
		Network::Internal::ProtocolConnect* header = reinterpret_cast<Network::Internal::ProtocolConnect*>(packet->Data);// work with the header
		header->channelCount=numofchannels;
		header->mtu = DEFAULTMTU;
		header->receivebandwidth =receivebandwidth;
		header->sendbandwidth =sendbandwidth;
		header->header.ChannelID=0;// always zero for connect
		header->header.command = Network::Internal::COMMAND_CONNECT;
		header->header.PeerID = (sessid << 13) | id;// this id is just our internal id.. it doesnt matter, because the server will ignore it on a connect
		header->header.SequenceNumber = packet->Id = Peers[id].I_Channels[0].ReliableOutId++;
		I_Locking_Send(Peers + id, packet->Data, packet->Size);// send the packet
		Peers[id].I_ProcessOutgoingReliable(0, packet);
		I_Client =true;
		I_Server=false;
		return Peers + id;
	}
	return NULL;
}

unsigned short UDP_Engine::I_GetIDFor(unsigned int addr, unsigned short port, unsigned char session){
	unsigned short id;
	if(I_PeerIds.empty()) {
		OUTPUT_NETWORK_DEBUG_MSG("Failed I_GetIDFor() because the I_PeerIds was empty. . returning");
		return MAXPEERCOUNT;// get out... no more ids
	}
#ifdef THREADING
	std::lock_guard<std::mutex> scoped(I_ContainersMutex);
#endif
	++NumOfPeers;
	std::pair<std::multimap<unsigned int, unsigned short>::iterator, std::multimap<unsigned int, unsigned short>::iterator> found = I_ConnectedIPs.equal_range(addr);// find out if this ip is connected already
	for(std::multimap<unsigned int, unsigned short>::iterator beg(found.first); beg!= found.second; beg++){// for the found ip addresses
		OUTPUT_NETWORK_DEBUG_MSG("Found a matching ip address in our list of connected users. Checking to see if it is already connected");
		if ((Peers[beg->second].State != Network::PEER_STATE_DISCONNECTED) & 
			(Peers[beg->second].Address == addr) & (Peers[beg->second].DestPort == port) & 
			(session == Peers[beg->second].SessionId)) {
			OUTPUT_NETWORK_DEBUG_MSG("Peer already issued a connect attempt. Cannot issue multiple attempts . . ");
			return MAXPEERCOUNT;// multiple connect attempts on the same peer, return invalid index
		} else{  
			OUTPUT_NETWORK_DEBUG_MSG("Different Connection attempt, issuing new id "); // else this is a new connection
			break;
		}
	}// this user has not connected to the address yet
	id = *I_PeerIds.begin();
	I_PeerIds.erase(I_PeerIds.begin());
	I_ConnectedIPs.insert(std::pair<unsigned int, unsigned short>(addr, id));// insert this user
	return id;
}
void UDP_Engine::I_ReleaseIDFor(cPeer* peer){
	unsigned int addr(peer->Address);
	unsigned short port(peer->DestPort);
	unsigned char session(peer->SessionId);
	unsigned short id(peer->Id);
	/*
	This may seem strange all the checking below, but let me explain. There are several scenarios. Imagine you connect to this server and then for what ever reason disconnect, then immediately try to reconnect.
	You will have the same ip address, but either the port will be different because you restarted the program to reconnect, or the session id will be different because the session id of the client increments for each connect attempt
	So, the checks below will prevent booting a valid user. The correct user, even if connecting from the same ip and the same port will be removed
	*/
#ifdef THREADING
	std::lock_guard<std::mutex> scoped(I_ContainersMutex);
#endif
	std::pair<std::multimap<unsigned int, unsigned short>::iterator, std::multimap<unsigned int, unsigned short>::iterator> found = I_ConnectedIPs.equal_range(addr);// find out if this ip is connected already
	for(std::multimap<unsigned int, unsigned short>::iterator beg(found.first); beg!= found.second; beg++){// for the found ip addresses	
		OUTPUT_NETWORK_DEBUG_MSG("Found a matching ip address in our list of connected users");
		if ((Peers[beg->second].Address == addr) & (Peers[beg->second].DestPort == port) & (session == Peers[beg->second].SessionId)) {
			OUTPUT_NETWORK_DEBUG_MSG("The Id :"<<id<<" has been reinserted into the id list");
			I_PeerIds.insert(id);// set will guarantee there are no two same keys	
			I_ConnectedIPs.erase(beg);// erase this user from the connectedips list
			--NumOfPeers;// decrease the user count
			return;
		} 
	}// this user has not connected to the address yet
	OUTPUT_NETWORK_DEBUG_MSG("Tried to release an ID that did not have valid address information. Did not recycle Id");
}
// a client processes this function, so it should be thread safe  . .  I cant imagine multiple threads all vying to get data and process it -- there just isnt enough data for a client
void UDP_Engine::I_ProcessVerifyConnect(cPeer* peer, char *pData,  unsigned short len, unsigned int receiveTime ){
	if((len != sizeof(Network::Internal::ProtocolConnect))) return;// bad data 
	Network::Internal::ProtocolConnect* header = reinterpret_cast<Network::Internal::ProtocolConnect*>(pData);// work with the header	
	unsigned short peerid = (header->header.PeerID & ~0xe000);
	peer->NextTimeout = Network::GetTime() + TIMEOUT;	
	peer->Id = peerid;// the ID we have is from the server.
	peer->Mtu = clamp<unsigned short>(header->mtu, MINMTU, MAXMTU);
	peer->Sendbandwidth = clamp<unsigned short>(header->sendbandwidth, MINPEERBANDWIDTH, UNLIMITEDBANDWIDTH);
	peer->Receivebandwidth = clamp<unsigned short>(header->receivebandwidth, MINPEERBANDWIDTH, UNLIMITEDBANDWIDTH);
	peer->ChannelCount= clamp<unsigned char>(header->channelCount, MINCHANNELCOUNT, MAXCHANNELCOUNT);
	peer->State = Network::PEER_STATE_ACKNOWLEDGING_CONNECT;
	// do a manual delete of the connect packet sent out. This is optional since the next ack will delete it, this just makes sure I dont try to resend it in the mean time
	// again, it doesnt matter if I do resend it, I just dont like lots of traffic heading around!

	peer->I_Channels[0].ACKPacket(0, Network::GetTime());
	std::list<Internal_DataPacket *>::iterator beg;

	++peer->I_Channels[0].ReliableInId;// incrment this becuase we received a reliable packet
	OUTPUT_NETWORK_DEBUG_MSG("Achknowledging connect request from the server ");
}
void UDP_Engine::I_ProcessConnect(cPeer* peer){
	OUTPUT_NETWORK_DEBUG_MSG("USER IS CONNECTED");
	peer->State = Network::PEER_STATE_CONNECTED;
	Connect(peer);// call the virtual function in case any processing needs to be done on a higher level
}
// the server calls this function on connection attempts
void UDP_Engine::I_ReceiveConnect( char *pData, unsigned short len, unsigned int address,  unsigned short port, unsigned int receiveTime ){// handle new connection attempts
	if(len != sizeof(Network::Internal::ProtocolConnect)) return;// bad data 

	Network::Internal::ProtocolConnect *connectheader = reinterpret_cast<Network::Internal::ProtocolConnect*>(pData);
	unsigned char sessionid = (connectheader->header.PeerID & 0xe000)>>13;// the session id is stored in the first three bits. This just adds a little extra help for different connections
	
	unsigned short id = I_GetIDFor(address, port, sessionid);
	if(id >=MAXPEERCOUNT)  return;// failed to get an id. Get out

	// the setup of these should be thread safe only because if a new user is connecting, I shouldnt be accessing that slot because I wouldnt know about it yet anyways...
	//The client sends how much bandwidth he is willing to receive. So, we set our SEND bandwidth not to overwhel the client
	connectheader->receivebandwidth = clamp<unsigned int>(connectheader->receivebandwidth, MINPEERBANDWIDTH, PerPeerSendBandwidth);// the servers sendbandwidth is the clients receivebandwidth
	connectheader->sendbandwidth = clamp<unsigned int>(connectheader->sendbandwidth, MINPEERBANDWIDTH, PerPeerReceiveBandwidth);// the servers sendbandwidth is the clients receivebandwidth
	connectheader->channelCount = clamp<unsigned char>(connectheader->channelCount, MINCHANNELCOUNT, MaxChannelCount);
	connectheader->mtu = clamp<unsigned short>(connectheader->mtu, MINMTU, MaxMTU);
	cPeer *peer = Peers +id;// set up the peer, clear everything and get it ready
	peer->I_SetupConnect(address, port, id, sessionid, connectheader->mtu, connectheader->receivebandwidth, connectheader->sendbandwidth, connectheader->channelCount);
	// now, send a responce to the user
	peer->SrcPort = port;
	++peer->I_Channels[0].ReliableInId;// incrment this becuase we received a reliable packet
	Internal_DataPacket* pack = new  Internal_DataPacket(pData, len, receiveTime, peer->I_Channels[0].ReliableOutId++);
	Network::Internal::ProtocolConnect* header = reinterpret_cast<Network::Internal::ProtocolConnect*>(pack->Data);// work with the header
	header->header.ChannelID = 0;
	header->header.command = Network::Internal::COMMAND_VERIFY_CONNECT;
	header->header.PeerID = (peer->SessionId << 13) | peer->Id;// the sessionid takes up the first 3 bits, the id takes up the rest of the unsigned short
	header->header.SequenceNumber = pack->Id;
	I_Locking_Send(peer, pack->Data, pack->Size);// send the packet
	peer->I_ProcessOutgoingReliable(0, pack);
	peer->State = Network::PEER_STATE_ACKNOWLEDGING_CONNECT;
	OUTPUT_NETWORK_DEBUG_MSG("Achknowledging connect request from the client ");
}
void UDP_Engine::I_SendCommand(cPeer* peer, unsigned char channel, unsigned char command){
	Internal_DataPacket* pack = new Internal_DataPacket(sizeof(Network::Internal::ProtocolCommandHeader));// we need to take ownership of the packet sent to usss...
	Network::Internal::ProtocolCommandHeader* header = reinterpret_cast<Network::Internal::ProtocolCommandHeader*>(pack->Data);// work with the header
	header->ChannelID = channel;
	header->command = command;
	header->PeerID = (peer->SessionId << 13) | peer->Id;// the sessionid takes up the first 3 bits, the id takes up the rest of the unsigned short
	header->SequenceNumber = pack->Id = peer->I_Channels[channel].ReliableOutId++;// get the next id for this packet
	OUTPUT_NETWORK_DEBUG_MSG("Sending a Command Packet with an id: "<<pack->Id<<" to peer id: "<<peer->Id<<" on Channel: "<<(unsigned short)channel);
	I_Locking_Send(peer, pack->Data, pack->Size);// send the packet
	peer->I_ProcessOutgoingReliable(channel, pack);		
}
#ifdef THREADING
void UDP_Engine::I_SendTo(Internal_DataPacket& packet, cPeer* peer , unsigned char channel, unsigned char command, std::atomic<unsigned short>& Id,
		void (cPeer::*process)(unsigned char channel, Internal_DataPacket* packet), unsigned short (cChannel::*increaseidby)(unsigned short inc), unsigned char fragcommand){	
#else 
void UDP_Engine::I_SendTo(Internal_DataPacket& packet, cPeer* peer , unsigned char channel, unsigned char command, unsigned short& Id,
		void (cPeer::*process)(unsigned char channel, Internal_DataPacket* packet), unsigned short (cChannel::*increaseidby)(unsigned short inc), unsigned char fragcommand){	
#endif
	// this is here first in case the packet needs to be fragmented, this way the packet fills its header 
	unsigned char actualchannel = (channel & 0x0f);
			
	Network::Internal::ProtocolStandard* header = reinterpret_cast<Network::Internal::ProtocolStandard*>(packet.Data);// work with the header
	header->header.ChannelID = channel;
	header->header.command = command;
	header->header.PeerID = (peer->SessionId << 13) | peer->Id;// the sessionid takes up the first 3 bits, the id takes up the rest of the unsigned short
	// fragmenting an unreliable packet is probably a sin or something, but i allow it. So..... user beware this is not guaranteeeeed!!
	
	if(packet.Size > peer->Mtu){// break up the packet into managable chunks for the peer
		// i decided to cap this because I use unsigned short ID numbers, and if there are more fragments than ids, the ids will wrap and errors will happen!
		// So, if users use the default MTU at 1400 bytes, the max allowable single packet is around 91MB, at the 576, its around 35MB. Which, for single sends is not a bad thing. 
		//if anyone is trying to send a single packet larger than that, they should probably rethink what they are doing, this network library is not a file service. A file service is a higher layer
		//of abstraction and should not be implemented at this level anyway. 
		if(packet.Size > static_cast<unsigned int>(peer->Mtu) * MAXNUMOFFRAGMENTS){
			OUTPUT_NETWORK_DEBUG_MSG("Error, The maximum allowable single packet is: "<<static_cast<unsigned int>(peer->Mtu) * MAXNUMOFFRAGMENTS<<" bytes for this peer. Cannot send!");
		}
		unsigned int bytecounter =0;
		
		cChannel* currchannl = peer->I_Channels + actualchannel;// DO NOT include the encryption part of the channel.
		unsigned short totalfragments = packet.Size / ( static_cast<unsigned int>(peer->Mtu) - static_cast<unsigned int>(sizeof(Network::Internal::ProtocolFragment)));
		// if the answer did not divide perfectly, it means integer division rounded down, so we must round up!
		if(packet.Size != (static_cast<unsigned int>(peer->Mtu) - sizeof(Network::Internal::ProtocolFragment)) * totalfragments) ++totalfragments;
		OUTPUT_NETWORK_DEBUG_MSG("Total Fragments that the packet is bring broken up into is: "<<totalfragments);
		unsigned short lastid = (currchannl->*increaseidby)(totalfragments);
		unsigned short startid = lastid - totalfragments;
		unsigned short startSequenceNumber =startid;
		unsigned int chunksize = static_cast<unsigned int>(peer->Mtu) - sizeof(Network::Internal::ProtocolFragment);
		unsigned int totallength = packet.Size;
		// this is so i can keep track of what I have copied so far
		for(char* copypointer = packet.Data; copypointer < packet.Data + packet.Size;  copypointer+=chunksize){
		
			unsigned int sizetoalloc = peer->Mtu;// this is the max size packet allowed to a peer
			if(copypointer + chunksize >= packet.Data + packet.Size){// we are on the last packet, so allocate the remaining 
				sizetoalloc = static_cast<unsigned int>(((packet.Data + packet.Size)-copypointer) + sizeof(Network::Internal::ProtocolFragment));
			} 
			Internal_DataPacket* pack = new Internal_DataPacket(sizetoalloc);
			memcpy(pack->Data + sizeof(Network::Internal::ProtocolFragment), copypointer, sizetoalloc - sizeof(Network::Internal::ProtocolFragment));
			pack->Size = pack->Capacity;// in this case, they should be equal
			Network::Internal::ProtocolFragment* fragheader = reinterpret_cast<Network::Internal::ProtocolFragment*>(pack->Data);// work with the header
			
			fragheader->fragmentCount = totalfragments;
			fragheader->totallength = totallength;
			fragheader->startSequenceNumber = startSequenceNumber;
			fragheader->header.ChannelID = actualchannel;// DO NOT include the encryption part of the channel.
			fragheader->header.command = fragcommand;
			fragheader->header.PeerID = (peer->SessionId << 13) | peer->Id;// the sessionid takes up the first 3 bits, the id takes up the rest of the unsigned short
			fragheader->header.SequenceNumber = pack->Id= startid++; // get the next id for this packet
			I_Locking_Send(peer, pack->Data, pack->Size);// send the packet
			(peer->*process)(channel, pack);
		}
		packet.free();
		
		memset(&packet, 0, sizeof(Internal_DataPacket));// zeero it out the packet so it wont delete the memory when its destructor is called
		return;// all done
	}
	Internal_DataPacket* pack = new Internal_DataPacket;// we need to take ownership of the packet sent to usss...
	memcpy(pack, &packet, sizeof(Internal_DataPacket));// cheap copy
	memset(&packet, 0, sizeof(Internal_DataPacket));// zeero it out the packet so it wont delete the memory when its destructor is called





	header->header.SequenceNumber = pack->Id = Id++;// get the next id for this packet
	OUTPUT_NETWORK_DEBUG_MSG("Sending a Packet with a size of: "<<pack->Size<<"an id: "<<pack->Id<<" to peer id: "<<peer->Id<<" on Channel: "<<(unsigned short)actualchannel);
	I_Locking_Send(peer, pack->Data, pack->Size);// send the packet
	(peer->*process)(actualchannel, pack);
}

/*
[beg, end)
*/
void UDP_Engine::Run(unsigned short beg, unsigned short end){ 
	for(cPeer* peer(Peers + beg); peer!=Peers + end; peer+=1){// for each peeer
		if(peer->State == Network::PEER_STATE_DISCONNECTED) continue;// skip disconnected users
		I_PreProcessing(peer);// do timeout work, and ping calculations
		switch(peer->State){
			case Network::PEER_STATE_ACKNOWLEDGING_DISCONNECT:// this is a disconnect event, do any last clean up and send a confirmation
				I_ProcessDisconnect(peer);// after this call, the peer is wiped and reset
				continue;
			case Network::PEER_STATE_ACKNOWLEDGING_CONNECT:// connect event
				I_ProcessConnect(peer);// set up the peer first before doing any work 
				break;
		};
		for(cChannel* channel(peer->I_Channels); channel != peer->I_Channels + peer->ChannelCount; channel++){ // for each channel
			I_PostProcessing(peer, channel);// post processing on each channel
			for(size_t i(0); i< channel->Packets.size(); i++){// for each packet waiting to be worked on in this channel
				ReceivePacket(channel->Packets[i], peer, channel - peer->I_Channels);
				delete channel->Packets[i];// delete the packet after the work is finished
			}
			channel->Packets.clear();// clear the packet buffer
		}
	}
}
void UDP_Engine::I_PreProcessing(cPeer* peer){
	float loss = peer->CalculateLoss();
	unsigned int time = Network::GetTime();
	if(loss > PACKETLOSSDISCONNECT){
		OUTPUT_NETWORK_DEBUG_MSG("Users packet loss: "<<loss<<"% is too high, disconnecting");// do not add code here to handle disconnect. Add any disconnect specific code to the virutal Disconnect(cPeer* peer) function you must write
		peer->State = Network::PEER_STATE_ACKNOWLEDGING_DISCONNECT;// the users packet loss is too high, disconect
	}
	if(peer->NextTimeout < time) {
		OUTPUT_NETWORK_DEBUG_MSG("User has timed out, disconnecting");// do not add code here to handle disconnect. Add any disconnect specific code to the virutal Disconnect(cPeer* peer) function you must write
		peer->State = Network::PEER_STATE_ACKNOWLEDGING_DISCONNECT;// the user timed out..
	}
	if(peer->NextPingTime < time){// ping user.. this keeps the connection alive
		peer->NextPingTime = time + PINGRATE;
		OUTPUT_NETWORK_DEBUG_MSG("Pinging Peer Id:"<<peer->Id);
		I_SendCommand(peer, 0, Network::Internal::COMMAND_PING);
	}
}
void UDP_Engine::I_PostProcessing(cPeer* peer, cChannel* channel){
	if(channel->BuildAck()) I_SendAcknowledge(channel, channel - peer->I_Channels, peer); // an ack buffer was built, get it and send it out
	Internal_DataPacket* packet=0;
	unsigned int t = Network::GetTime();
	Network::Internal::ProtocolCommandHeader* header;
	while( channel->Pop_ReliableIn  (&packet)) {// process the reliable packets that we have waiting in our buffer
		header = reinterpret_cast<Network::Internal::ProtocolCommandHeader*>(packet->Data);
		OUTPUT_NETWORK_DEBUG_MSG("Processing a Reliable packet of the Type :"<<Network::Internal::CommandsToString[header->command] );
		switch(header->command){
			case(Network::Internal::COMMAND_RELIABLE):// this is a single reliable packet... insert it into the waiting packets
				channel->Packets.push_back(new cDataPacket(*packet));// this copies the data out so it is not destroyed in the delete a few lines below
				break;
			case(Network::Internal::COMMAND_RELIABLE_FRAGMENT):
				channel->ProcessReliableFragment(packet);
				continue;// do not delete the packet in this case... we are still holding it for further processing
			case(Network::Internal::COMMAND_INIT_ENCRYPTION):// Initialize encryption
				I_VerifySession(peer, packet);
				break;
			case(Network::Internal::COMMAND_ENCRYPTED_RELIABLE_PACKET):
				if(I_Decrypt(peer, packet))// only if the decrypt was successfull
					channel->Packets.push_back(new cDataPacket(*packet));// this copies the data out so it is not destroyed in the delete a few lines below
				break;
		};
		
		delete packet;// delete the packet in any case.. we are done with it
	}
	while( channel->Pop_UnReliableIn(&packet)){// process unreliable packets in the buffer
		header = reinterpret_cast<Network::Internal::ProtocolCommandHeader*>(packet->Data);
		Network::Internal::ProtocolAcknowledge* ackheader;
		OUTPUT_NETWORK_DEBUG_MSG("Processing an UnReliable packet of the Type :"<<Network::Internal::CommandsToString[header->command] );
		switch(header->command){
			case(Network::Internal::COMMAND_ACKNOWLEDGE):
				ackheader = reinterpret_cast<Network::Internal::ProtocolAcknowledge*>(packet->Data);
				channel->ProcessIncomingACKs(packet->Data + sizeof(Network::Internal::ProtocolAcknowledge), packet->Size - sizeof(Network::Internal::ProtocolAcknowledge), packet->FirstTime);
				peer->ClockTime = (peer->GetAveragePing( 50 ) >>1) + ackheader->SentTime;// try to sync the time with the peer connected
				peer->TimeClockTime = t;// update the time
				break;
			case(Network::Internal::COMMAND_UNRELIABLE):
				channel->Packets.push_back(new cDataPacket(*packet));// this copies the data out so it is not destroyed in the delete a few lines below
				break;
			case(Network::Internal::COMMAND_DISCONNECT):
				peer->State = Network::PEER_STATE_ACKNOWLEDGING_DISCONNECT;
				break;
			case(Network::Internal::COMMAND_UNRELIABLE_FRAGMENT):
				channel->ProcessUnReliableFragment(packet);
				continue;// do not delete the packet in this case... we are still holding it for further processing
			case(Network::Internal::COMMAND_ENCRYPTED_UNRELIABLE_PACKET):
				if(I_Decrypt(peer, packet))
					channel->Packets.push_back(new cDataPacket(*packet));// this copies the data out so it is not destroyed in the delete a few lines below
				break;
		};
		delete packet;
	}
	I_ResendPackets(peer, channel);
}


void UDP_Engine::I_ResendPackets(cPeer* peer, cChannel* channel){
	unsigned int ping = peer->GetAveragePing(70);// get the average of 70% of the users worst ping times...
	unsigned int time = Network::GetTime();
	if(ping < 40) ping = 40;// cap this because when the ping gets to low.. small fluctuations in the ping can make the server thing the peer's packet loss is too high
	for(std::list<Internal_DataPacket*>::iterator beg(channel->ReliableOut.begin()); beg!= channel->ReliableOut.end(); beg++){// for all of the reliable packets in the queue
		/*
		imple but effective method for resending packets. It would be a bad idea to hammer a peer that is having connection issues
		So, for each time I have to resend the packet, I multiply that by their ping. So, if the ping is 300 ms, and this is the second time I am trying to send the packet
		then I wont send the packet until 300*2=600ms. A third loss on the same packet would be a 300*3=900. Servicing bad connections is not a good idea anyway
		*/
		if( time > (*beg)->LastTime + ping*static_cast<unsigned int>((*beg)->TimesSent)){// resend this packet.. it is time!!
#ifdef THREADING
			if(I_Locking_Send(peer, (*beg)->Data, (*beg)->Size)){// send the packet
#else
			if(I_NonLocking_Send(peer, (*beg)->Data, (*beg)->Size)){// send the packet
#endif			
				++PacketsLost;
				OUTPUT_NETWORK_DEBUG_MSG("Resending Reliable packet with id: "<<(*beg)->Id);
				// only update the information if the packet was send out
				(*beg)->LastTime = time;
				(*beg)->TimesSent+=1;// increment this
			}
		} else break;// once we get a packet that is too new, break out of this loop
	}
}
void UDP_Engine::I_SendAcknowledge(cChannel* channel, size_t channelindex, cPeer* peer){
	Network::Internal::ProtocolAcknowledge* header = reinterpret_cast<Network::Internal::ProtocolAcknowledge*>(channel->AckBuffer);// work with the header
	header->header.ChannelID = static_cast<unsigned char>(channelindex);
	header->header.command = Network::Internal::COMMAND_ACKNOWLEDGE;
	header->header.PeerID = (peer->SessionId << 13) | peer->Id;// the sessionid takes up the first 3 bits, the id takes up the rest of the unsigned short
	header->header.SequenceNumber = channel->UnReliableOutId++;
	header->SentTime =  Network::GetTime();
	I_Locking_Send(peer, channel->AckBuffer, sizeof(Network::Internal::ProtocolAcknowledge) + channel->AckLength);// send the packet
}

void UDP_Engine::I_ProcessDisconnect(cPeer* peer){
	if(peer->State == Network::PEER_STATE_DISCONNECTED) return;
	// upon receiving a disconnect, send a responce just to be nice, but after this, we will clear this peer
	Disconnect(peer);// call the virtual function in case any cleanup needs to be done on a higher level
	char pa[sizeof(Network::Internal::ProtocolCommandHeader)];
	Network::Internal::ProtocolCommandHeader* header = reinterpret_cast<Network::Internal::ProtocolCommandHeader*>(pa);
	header->ChannelID = 0;
	header->command = Network::Internal::COMMAND_DISCONNECT;
	header->SequenceNumber = peer->I_Channels[0].UnReliableOutId++;
	header->PeerID = (peer->SessionId << 13) | peer->Id;// the sessionid takes up the first 3 bits, the id takes up the rest of the unsigned short
	I_Locking_Send(peer, pa, sizeof(Network::Internal::ProtocolCommandHeader));// send the packet
	if(I_Server)// easier to put this here. A client does not release an id, nor does a client get an id. This is a server thing because of the multiple connections
		I_ReleaseIDFor(peer);
	OUTPUT_NETWORK_DEBUG_MSG("Peer with an id: "<<peer->Id<<" is now Disconnected");
	peer->I_clear();// this peer is now reset
	
}

void UDP_Engine::InitializeEncryption(cPeer* peer){
#if defined(USEENCRYPTION)
	peer->EncryptionState = Network::PEER_STATE_EXCHANGING_KEYS;

	peer->EncryptionInfo.Init(I_Client);

	peer->EncryptionInfo.fhmqv->GenerateStaticKeyPair(rnd, peer->EncryptionInfo.staticprivatekey, peer->EncryptionInfo.staticpublickey);
	peer->EncryptionInfo.fhmqv->GenerateEphemeralKeyPair(rnd, peer->EncryptionInfo.ephemeralprivatekey, peer->EncryptionInfo.ephemeralpublickey);
	
	Internal_DataPacket* pack = new Internal_DataPacket(sizeof(Network::Internal::ProtocolCommandHeader)+ peer->EncryptionInfo.staticpublickey.SizeInBytes() + peer->EncryptionInfo.ephemeralpublickey.SizeInBytes());
	Network::Internal::ProtocolCommandHeader* header = reinterpret_cast<Network::Internal::ProtocolCommandHeader*>(pack->Data);// work with the header
	header->ChannelID = 0;
	header->command = Network::Internal::COMMAND_INIT_ENCRYPTION;
	header->PeerID = (peer->SessionId << 13) | peer->Id;// the sessionid takes up the first 3 bits, the id takes up the rest of the unsigned short
	header->SequenceNumber = pack->Id = peer->I_Channels[0].ReliableOutId++;// get the next id for this packet
	OUTPUT_NETWORK_DEBUG_MSG("Initializing Encryption Packet id: "<<pack->Id<<" peer id: "<<peer->Id);

	char* da = pack->Data + sizeof(Network::Internal::ProtocolCommandHeader);

	memcpy(da, peer->EncryptionInfo.staticpublickey.BytePtr(), peer->EncryptionInfo.staticpublickey.SizeInBytes());// the public keys are 256 bytes long
	da +=peer->EncryptionInfo.staticpublickey.SizeInBytes();
	memcpy(da, peer->EncryptionInfo.ephemeralpublickey.BytePtr(), peer->EncryptionInfo.ephemeralpublickey.SizeInBytes());// the public keys are 256 bytes long
	I_Locking_Send(peer, pack->Data, pack->Size);// send the packet
	peer->I_ProcessOutgoingReliable(0, pack);	
#endif
}

void UDP_Engine::I_VerifySession(cPeer* peer, Internal_DataPacket* packet){
#if !defined(USEENCRYPTION)
	// SecureConnectionEstablished is not called in purpose so that users cannot unintentionally extablish a secure connection by accidentally turning encryption off
	OUTPUT_NETWORK_DEBUG_MSG("A secure connection was attemped, but encryption is currently not supported in ths build. If this is intentional, ignore this message. If not, turn encryption on in the NetworkUtils.h by defining USEENCRYPTION");
	return;
#else
		if(peer->EncryptionState == Network::PEER_STATE_ENCRYPTION_NOT_READY ) { // if we have not sent our keys, do it now
			InitializeEncryption(peer);
		} else if(peer->EncryptionState == Network::PEER_STATE_ENCRYPTION_READY){ // this means multiple attempts to exchange keys, or keys already exchanged.. could be an attempt to crash the server
			return DisconnectPeer(peer);// disconnect in this case
		}
		if(peer->EncryptionInfo.staticpublickey.SizeInBytes() + peer->EncryptionInfo.ephemeralpublickey.SizeInBytes() != packet->Size - sizeof(Network::Internal::ProtocolCommandHeader)){// if the key sizes do not match.. bad!!   divide by two == >>1 
			OUTPUT_NETWORK_DEBUG_MSG("Packet size is not correct to initiate a secure session, Disconnecting Peer");
			OUTPUT_NETWORK_DEBUG_MSG("This key length should be: "<<peer->EncryptionInfo.staticpublickey.SizeInBytes() + peer->EncryptionInfo.ephemeralpublickey.SizeInBytes()<<" but, received key length was: "<<(size_t)(packet->Size - sizeof(Network::Internal::ProtocolCommandHeader)));
			return DisconnectPeer(peer); 
		}

		SecByteBlock sharedsecret(peer->EncryptionInfo.fhmqv->AgreedValueLength());
		unsigned char* firstkey = (unsigned char*)packet->Data + sizeof(Network::Internal::ProtocolCommandHeader);

		bool verified = peer->EncryptionInfo.fhmqv->Agree(sharedsecret, peer->EncryptionInfo.staticprivatekey, peer->EncryptionInfo.ephemeralprivatekey, firstkey, firstkey + peer->EncryptionInfo.staticpublickey.SizeInBytes());
		peer->EncryptionInfo.clear_keyexchange();// make sure to clear the public and private keys, they are no longer needed
		// we are switching to a symmetric AES128 key now
		if(verified){
			Integer ssa;
			ssa.Decode(sharedsecret.BytePtr(), sharedsecret.SizeInBytes());
			OUTPUT_NETWORK_DEBUG_MSG("Secured Connection made!!");
			// Calculate a SHA-256 hash over the Diffie-Hellman session key
			SHA256().CalculateDigest(peer->EncryptionInfo.AESKey, sharedsecret, sharedsecret.size()); 
			peer->EncryptionState=Network::PEER_STATE_ENCRYPTION_READY;
			SecureConnectionEstablished(peer);
		} else {
			OUTPUT_NETWORK_DEBUG_MSG("Un Secured Connection attempt!!, Disconnecting Peer");
			DisconnectPeer(peer); 
		}

#endif
}


void UDP_Engine::ReliableEncryptedSendTo(cPeer* peer , cDataPacket& packet,  unsigned char channel){
	if(packet.Length == 0) return ReliableSendTo(peer, packet, channel); // if there is no data to send, skip encyption stuff
#if defined(USEENCRYPTION)
	unsigned int oldlen=packet.Length + AES::BLOCKSIZE;
	packet.resize(oldlen);
	packet.resizetoblocksize(AES::BLOCKSIZE);
	unsigned char bytestodata = static_cast<unsigned char>(packet.Length- oldlen)<<4;// get the number of bytes that the packet was resized and shift it into position 
	GCM<AES>::Encryption Encryptor;
	try{// Crypto++ loves throwing stuff around! If any errors occur, it likely that something is seriously screwed up on our part
		rnd.GenerateBlock((byte*)packet.Data  + packet.Length - AES::BLOCKSIZE, AES::BLOCKSIZE);
		Encryptor.SetKeyWithIV(peer->EncryptionInfo.AESKey, SHA256::DIGESTSIZE, (byte*)packet.Data  + packet.Length - AES::BLOCKSIZE);
		Encryptor.ProcessData(((byte*)packet.Data), ((byte*)packet.Data), packet.Length-AES::BLOCKSIZE);// number of bytes to encrypt is always 16 less than the length
    } catch( CryptoPP::InvalidArgument e ) {
		OUTPUT_NETWORK_DEBUG_MSG("Caught InvalidArgument..."<<e.what());
		return DisconnectPeer(peer);
    } catch( CryptoPP::Exception e ) {
        OUTPUT_NETWORK_DEBUG_MSG("Caught Exception..."<< e.what() );
		return DisconnectPeer(peer);
    }
#if defined(NETWORKDEBUG)
	std::string outest(packet.Data, 20);// get the first 20 bytes of data to do a sanity check to verify the data is encrypted
#endif
	I_SendTo(packet.Internal_Packet, peer, channel | bytestodata, Network::Internal::COMMAND_ENCRYPTED_RELIABLE_PACKET, peer->I_Channels[channel].ReliableOutId, &cPeer::I_ProcessOutgoingReliable, &cChannel::IncreaseOutGoingReliableIdBy, Network::Internal::COMMAND_RELIABLE_FRAGMENT);
	packet.Data =0;
	packet.Length=0;

	OUTPUT_NETWORK_DEBUG_MSG("Encryption complete . . . Sanity Check of first 20 bytes is" + outest);
#else 
	OUTPUT_NETWORK_DEBUG_MSG("ReliableEncryptedSendTo called, with no encryption support, will disconect the peer now.");
	DisconnectPeer(peer);
#endif
}
void UDP_Engine::UnreliableEncryptedSendTo(cPeer* peer ,cDataPacket& packet,  unsigned char channel){
	if(packet.Length == 0) return UnreliableSendTo(peer, packet, channel); // if there is no data to send, skip encyption stuff
#if defined(USEENCRYPTION)
	unsigned int oldlen=packet.Length + AES::BLOCKSIZE;
	packet.resize(oldlen);
	packet.resizetoblocksize();
	unsigned char bytestodata = static_cast<unsigned char>(packet.Length- oldlen)<<4;// get the number of bytes that the packet was resized and shift it into position 
	GCM<AES>::Encryption Encryptor;
	try{// Crypto++ loves throwing stuff around! If any errors occur, it likely that something is seriously screwed up on our part
		rnd.GenerateBlock((byte*)packet.Data  + packet.Length - AES::BLOCKSIZE, AES::BLOCKSIZE);
		Encryptor.SetKeyWithIV(peer->EncryptionInfo.AESKey, SHA256::DIGESTSIZE, (byte*)packet.Data  + packet.Length - AES::BLOCKSIZE);
		Encryptor.ProcessData(((byte*)packet.Data), ((byte*)packet.Data), packet.Length-AES::BLOCKSIZE);// number of bytes to encrypt is always 16 less than the length
    } catch( CryptoPP::InvalidArgument& e ) {
		OUTPUT_NETWORK_DEBUG_MSG("Caught InvalidArgument..."<<e.what());
		return DisconnectPeer(peer);
    } catch( CryptoPP::Exception& e ) {
        OUTPUT_NETWORK_DEBUG_MSG("Caught Exception..."<< e.what() );
		return DisconnectPeer(peer);
    }
#if defined(NETWORKDEBUG)
	std::string outest(packet.Data, 20);// get the first 10 bytes of data to do a sanity check to verify the data is encrypted
#endif

	I_SendTo(packet.Internal_Packet, peer, channel | bytestodata, Network::Internal::COMMAND_ENCRYPTED_UNRELIABLE_PACKET, peer->I_Channels[channel].UnReliableOutId, &cPeer::I_ProcessOutgoingUnreliable, &cChannel::IncreaseOutGoingUnReliableIdBy, Network::Internal::COMMAND_UNRELIABLE_FRAGMENT);
	packet.Data =0;
	packet.Length=0;
	OUTPUT_NETWORK_DEBUG_MSG("Encryption complete . . . Sanity Check (should be jumbled) of first 20 bytes is" + outest);
#else 
	OUTPUT_NETWORK_DEBUG_MSG("UnreliableEncryptedSendTo called, with no encryption support, will disconect the peer now.");
	DisconnectPeer(peer);
#endif
}


bool UDP_Engine::I_Decrypt(cPeer* peer, Internal_DataPacket* packet){
#if defined(USEENCRYPTION)
	size_t multiple =  (packet->Size -sizeof(Network::Internal::ProtocolStandard)) / AES::BLOCKSIZE;// do not include the header. Integer division rounds down
	if(( multiple * AES::BLOCKSIZE !=  (packet->Size -sizeof(Network::Internal::ProtocolStandard)) ) |
		(packet->Size -sizeof(Network::Internal::ProtocolStandard) < AES::BLOCKSIZE*2)){// the packet is not correctly sized..
		OUTPUT_NETWORK_DEBUG_MSG("Packet is not the correct size for encryption. Received a packet with a size "<<packet->Size);
		return false;
	}
	GCM<AES>::Decryption Decryptor;
	try{// Crypto++ loves throwing stuff around! If any errors occur, it likely that the peer is messing with us, disconnect 
		Decryptor.SetKeyWithIV(peer->EncryptionInfo.AESKey, SHA256::DIGESTSIZE, (byte*)packet->Data  + packet->Size - AES::BLOCKSIZE);
		Decryptor.ProcessData((byte*)packet->Data+sizeof(Network::Internal::ProtocolStandard), (byte*)packet->Data+sizeof(Network::Internal::ProtocolStandard), packet->Size - sizeof(Network::Internal::ProtocolStandard) - AES::BLOCKSIZE);
	} catch( CryptoPP::HashVerificationFilter::HashVerificationFailed& e ){
		OUTPUT_NETWORK_DEBUG_MSG("Caught HashVerificationFailed..."<< e.what());
		DisconnectPeer(peer);
		return false;
    } catch( CryptoPP::InvalidArgument& e ) {
		OUTPUT_NETWORK_DEBUG_MSG("Caught InvalidArgument..."<<e.what());
		DisconnectPeer(peer);
		return false;
    } catch( CryptoPP::Exception& e ) {
        OUTPUT_NETWORK_DEBUG_MSG("Caught Exception..."<< e.what() );
		DisconnectPeer(peer);
		return false;
    }
	Network::Internal::ProtocolStandard* header = reinterpret_cast<Network::Internal::ProtocolStandard*>(packet->Data);// work with the header
	packet->Size -= static_cast<unsigned int>( (header->header.ChannelID & 0xf0 )>>4);// this adjusts the packets correct size for encryption
	packet->Size -=AES::BLOCKSIZE;
	return true;
#else 
	OUTPUT_NETWORK_DEBUG_MSG("I_Decrypt was called without encryption supprt, this is a big error. The peer will be forced to disconnect now");
	DisconnectPeer(peer);
	return false;// this is here intentionally because if this is called, and encryption is not supported, it means something wrong is occuring and the peer must disconnect
#endif
}
