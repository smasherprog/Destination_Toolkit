#include "stdafx.h"
#include "cChannel.h"
#include "../Allocator_Lib/cAllocator.h"

void cChannel::clear(){
	BuildAckPacket=false;
	ReliableOutId=ReliableInId=UnReliableOutId=UnReliableInId=AckLength=0;
	ReliableFragmentId=~0;// this is maxint
	ReliableFragmentTotalLength = ReliableFragmentCount=0;
	UnReliableFragmentId=~0;// this is maxint
	UnReliableFragmentTotalLength = UnReliableFragmentCount=0;
	deletelist(ReliableOut);
	deletelist(ReliableInOrdered);
	deletelist(ReliableInUnOrdered);
	deletelist(UnReliableIn);

	deletelist(ReliableFragments);
	deletelist(UnReliableFragments);
	deletelist(Packets);

	PingIndex=PacketsLostIndex=0;
}

void cChannel::ProcessOutgoingReliable(Internal_DataPacket* packet){	
	packet->TimesSent =1.0f;
	packet->LastTime = packet->FirstTime = Network::GetTime();
#ifdef THREADING
	std::lock_guard<std::mutex> scoped(ReliableOutMutex);
	ReliableOut.push_back(packet);
#else
	ReliableOut.push_back(packet);
#endif
}
	
void cChannel::ProcessIncomingReliable(char* data, const unsigned short len, const unsigned int recvtime, const unsigned short id){
	if( ReliableInId > id  ){ 
		OUTPUT_NETWORK_DEBUG_MSG("Failed ProcessIncomingReliable, Excpeted: "<<ReliableInId<<"  Recived"<<id); 
		return;// if the packet we received has already been processed, ignore it
	} else {
		OUTPUT_NETWORK_DEBUG_MSG("ProcessIncomingReliable, Excpeted: "<<ReliableInId<<"  Recived"<<id); 
	}
	BuildAckPacket=true;
#ifdef THREADING
	std::lock_guard<std::mutex> scoped(ReliableInMutex);// the lock will release when the function exits
#endif
	if( ReliableInId == id){
		// This packet is the next ordered packet. Add it to the ordered list and then move all unordered that can be moved to the ordered list.
		Internal_DataPacket* packet = new Internal_DataPacket(data, len, recvtime, id);
		ReliableInOrdered.push_back(packet);
		ReliableInId++;

		while( !ReliableInUnOrdered.empty()){
			packet = ReliableInUnOrdered.front();
			if(ReliableInId != packet->Id ) break;
			ReliableInUnOrdered.pop_front();
			ReliableInOrdered.push_back( packet );
			ReliableInId++;
		}
	} else {  // ReliableInId > packet->Id
		// Out of order.  Sort into the list.
		std::list<Internal_DataPacket *>::iterator iPacket;
		bool  bExists = false;
		for( iPacket = ReliableInUnOrdered.begin(); iPacket != ReliableInUnOrdered.end(); ++iPacket ) {
			if( (*iPacket)->Id == id ){// Already in list - get out now!
				bExists = true;
				break;
			}
			if( (*iPacket)->Id > id ) break;
		}
		if( !bExists){// the packet is not in the list.. insert it
			// We've gone 1 past the spot where pPacket belongs.  Back up and insert.
			ReliableInUnOrdered.insert( iPacket, new Internal_DataPacket(data, len, recvtime, id) );
		}
	}
}

void  cChannel::ProcessIncomingUnReliable(char* data, const unsigned short len, const unsigned int recvtime, const unsigned short id){
	if(id < UnReliableInId ) return;
	OUTPUT_NETWORK_DEBUG_MSG("ProcessIncomingUnReliable, Excpeted: "<<UnReliableInId<<"  Recived"<<id); 
	UnReliableInId = id +1;
#ifdef THREADING
	std::lock_guard<std::mutex> scoped(UnReliableInMutex);// this is for both ordered and un ordered containes, since they are usually worked on together
#endif
	UnReliableIn.push_back(new Internal_DataPacket(data, len, recvtime, id));
}
void  cChannel::ProcessIncomingACKs(char* data, unsigned int len, unsigned int receiveTime){
	// Get the number of ACKs in this message, not counting the base ACK.
	
	unsigned int         basePacketID;
	unsigned char * ptr = (unsigned char *)data;
	memcpy( &basePacketID, ptr, sizeof( unsigned int ) );// Get the base packet ID, which indicates all the ordered packets received so far.
	ptr += sizeof( unsigned int );
	unsigned char numAcks = *ptr++;// Get the number of additional ACKs.
	unsigned int ackID = GetLowestReliableOutId();

	OUTPUT_NETWORK_DEBUG_MSG("Received Ack: Ack low= "<<ackID<<"base= "<<basePacketID<<"end="<<basePacketID + numAcks );
	while( ackID < basePacketID ){
		ACKPacket( ackID, receiveTime );
		ackID++;
	}
	unsigned char mask = 0x80;
	while( ackID < basePacketID + numAcks ){
		if( mask == 0 ) {
			mask = 0x80;
			ptr++;
		}
		if( ( *ptr & mask ) != 0 )
			ACKPacket( ackID, receiveTime );
		mask >>= 1;
		ackID++;
	}
}
void cChannel::ACKPacket( unsigned int packetID, unsigned int receiveTime ){// no lock needed here, this is called per peer
	OUTPUT_NETWORK_DEBUG_MSG("Acking Packet "<<packetID );
	Internal_DataPacket *pPacket=0;
	std::list<Internal_DataPacket *>::iterator beg;
	for(beg =ReliableOut.begin(); beg != ReliableOut.end(); ++beg ){
		if( (*beg)->Id == packetID )
			pPacket = *beg;
			break;
	}
	if( pPacket == NULL ) return;	
	if( ++*PingIndex >= PING_RECORDLENGTH - 2) *PingIndex=0;// wrap ping index
	if( ++*PacketsLostIndex >= PING_RECORDLENGTH - 2) *PacketsLostIndex=0;// wrap ping index
	*TotalPacketsLost-=PacketsLost[*PacketsLostIndex];// remove the last number of times the last packet was sent
	Pings[*PingIndex]= receiveTime - pPacket->FirstTime;
	PacketsLost[*PacketsLostIndex] = pPacket->TimesSent;
	*TotalPacketsLost+=pPacket->TimesSent;// add how many times this packet was sent to the running total
	OUTPUT_NETWORK_DEBUG_MSG("Deleting packet due to an ack received "<<(*beg)->Id);
	delete pPacket;
	ReliableOut.erase(beg);
}	
bool cChannel::BuildAck(){// no locks needed here.. only called per peer
	if(!BuildAckPacket) return false;
	BuildAckPacket=false;
	// Build the new ACK message.

	unsigned int lowest = ReliableInId;// get the current id
	unsigned int highest = GetHighestReliableInId();

	// Cap the highest so as not to overflow the ACK buffer
	// (or spend too much time building ACK messages).
	if( highest > lowest + ACK_MAXPERMSG ) highest = lowest + ACK_MAXPERMSG;

	unsigned char *ptr = (unsigned char *)AckBuffer + sizeof(Network::Internal::ProtocolAcknowledge);
	// Send the base packet ID, which is the ID of the last ordered packet received.
	memcpy( ptr, &lowest, sizeof( unsigned int ) );
	ptr += sizeof( unsigned int );
	// Add the number of additional ACKs.
	*ptr++ = highest - lowest;
	unsigned int ackID = lowest;
	unsigned char mask = 0x80;
	OUTPUT_NETWORK_DEBUG_MSG("Building Ack Packet: Ack low= "<<ackID<<"base= "<<lowest<<"end="<<highest);
	std::list<Internal_DataPacket*>::iterator beg= ReliableInUnOrdered.begin();
	while( ackID < highest ){
		if( mask == 0 ){
			mask = 0x80;
			ptr++;
		}
		bool found = false;
		for(; beg!= ReliableInUnOrdered.end(); beg++){
			if( (*beg)->Id == ackID) {
				found= true;
				break;
			}
		}
		if( found == true ){
			*ptr |= mask;  // There is
		}else{
			*ptr &= ~mask;  // There isn't
		}
		mask >>= 1;
		ackID++;
	}
	// Record the ammount of the ackBuffer used.
	AckLength = static_cast<unsigned short>(( ptr - ( (unsigned char *)AckBuffer +sizeof(Network::Internal::ProtocolAcknowledge) )  ) + ( mask != 0 ));
	return true;
}

bool cChannel::Pop_ReliableIn(Internal_DataPacket** packet){ 
	if(ReliableInOrdered.empty()) return false; 
	*packet = ReliableInOrdered.front(); 
	ReliableInOrdered.pop_front();	

	return true; 
}
bool cChannel::Pop_UnReliableIn(Internal_DataPacket** packet){ 
	if(UnReliableIn.empty()) return false; 
	*packet = UnReliableIn.front(); 
	UnReliableIn.pop_front();

	return true; 
}

void cChannel::ProcessFragment(Internal_DataPacket* packet, std::vector<Internal_DataPacket*>& fragments, unsigned int &fragmentcount, unsigned int &fragmenttotalLength, unsigned int &fragmentid, std::list<Internal_DataPacket*>& furtherprocess){
	Network::Internal::ProtocolFragment* header = reinterpret_cast<Network::Internal::ProtocolFragment*>(packet->Data);// work with the header
	if(header->fragmentCount > MAXNUMOFFRAGMENTS) return;// invalid.. cannot excced fragment count
	// check the fragment to ensure it is not junk data
	if( static_cast<unsigned int>(header->startSequenceNumber) == fragmentid){// already tryiing to assemble this fragment, continue
		if(static_cast<unsigned int>(header->startSequenceNumber) != fragmentid || static_cast<unsigned int>(header->fragmentCount) != fragmentcount  || fragmenttotalLength != header->totallength) {// check for funny stuff, and destroy all fragments if so
			for(size_t i(0); i< fragments.size(); i++){// funny stuff found, delete all the fragments
				delete fragments[i];
			}
			OUTPUT_NETWORK_DEBUG_MSG("BAD FRAGMENT ");
			//in any case, we need to reset the fragment buffer and return
			fragmentid=~0;// this is maxint
			fragmenttotalLength = fragmentcount=0;
			fragments.clear();
			delete packet;// we take ownership if the packet, so delete it when done
		} else if(fragments.size() +1 == fragmentcount){// this should be the last fragment.. assemble it and add it to our packets	
			OUTPUT_NETWORK_DEBUG_MSG("LAST FRAGMENT");
			fragments.push_back(packet);// we take the packet here..
			char* data = (char*)NLD_Allocator::NLD_malloc(fragmenttotalLength);
			char* tmp = data;
			for(size_t i(0); i< fragments.size(); i++){
				memcpy(tmp, fragments[i]->Data + sizeof(Network::Internal::ProtocolFragment), fragments[i]->Size - sizeof(Network::Internal::ProtocolFragment));
				tmp+=fragments[i]->Size - sizeof(Network::Internal::ProtocolFragment);// bytes copied
				delete fragments[i];// all fragments are freed here
			}
			if(tmp - data != fragmenttotalLength){// there was an error somewhere in the fragment... destroy it and reset the fragment buffer
				NLD_Allocator::NLD_free(data);
				OUTPUT_NETWORK_DEBUG_MSG("CORRUPT FRAGMENT");
			} else {// correctly assembled
				OUTPUT_NETWORK_DEBUG_MSG("CORRECTLY ASSEMBLED ");
				cDataPacket* p;
				Internal_DataPacket* ip;
				Network::Internal::ProtocolStandard* header = reinterpret_cast<Network::Internal::ProtocolStandard*>(data);// work with the header
				switch(header->header.command){// make sure we put the packet in the correct place
					case(Network::Internal::COMMAND_RELIABLE):
					case(Network::Internal::COMMAND_UNRELIABLE):
						p = new cDataPacket;
						p->Internal_Packet.Data = data;
						p->Length = fragmenttotalLength - sizeof(Network::Internal::ProtocolStandard);
						p->Internal_Packet.Capacity =p->Internal_Packet.Size = fragmenttotalLength;
						p->Data = data + sizeof(Network::Internal::ProtocolStandard);
						Packets.push_back(p);	
						break;
					default:// this means the packet needs further processing, i.e. encrypted packets
						ip=new Internal_DataPacket;
						ip->Data = data;
						ip->Capacity = ip->Size = fragmenttotalLength;
						furtherprocess.push_back(ip);// this packet needs further processing
				};
				
			}
			//in any case, we need to reset the fragment buffer and return
			fragmentid=~0;// this is maxint
			fragmenttotalLength = fragmentcount=0;
			fragments.clear();
		} else if(fragments.back()->Id != header->header.SequenceNumber -1 ){// since unreliable and reliable are processed through here, check for missing fragments, if there are discard all
			for(size_t i(0); i< fragments.size(); i++){// funny stuff found, delete all the fragments
				delete fragments[i];
			}
			OUTPUT_NETWORK_DEBUG_MSG("BAD FRAGMENT, missing one. Probably an unreliable send.. use reliable next time! ");
			//in any case, we need to reset the fragment buffer and return
			fragmentid=~0;// this is maxint
			fragmenttotalLength = fragmentcount=0;
			fragments.clear();
			delete packet;// delete the packet too!!
		} else {// next fragment.. add it and get out
			fragments.push_back(packet);
		}
	} else if(fragments.empty()){// new fragment
		OUTPUT_NETWORK_DEBUG_MSG("First time assembling packet ");
		fragmentid = static_cast<unsigned int>(header->startSequenceNumber);
		fragmentcount = static_cast<unsigned int>(header->fragmentCount);
		fragmenttotalLength = header->totallength;
		Internal_DataPacket* pack = new Internal_DataPacket;
		memcpy(pack, packet, sizeof(Internal_DataPacket));
		memset(packet, 0, sizeof(Internal_DataPacket));
		fragments.push_back(pack);
	}
}