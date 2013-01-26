#include "stdafx.h"
#include "cDataPacket.h"
#include "NetworkUtils.h"
#include "../Allocator_Lib/cAllocator.h"

void* Internal_DataPacket::operator new(size_t sz){
	return NLD_Allocator::NLD_malloc((unsigned int)sz);
}
void Internal_DataPacket::operator delete(void* ptr){
	NLD_Allocator::NLD_free(ptr);
}
void Internal_DataPacket::free(){
	NLD_Allocator::NLD_free(Data);	
	Data =0;
}

Internal_DataPacket::Internal_DataPacket(unsigned int len) : Data((char*)NLD_Allocator::NLD_malloc(len)),  TimesSent(0.0f), Size(len), Capacity(len), Id(0), FirstTime(0), LastTime(0) {}
Internal_DataPacket::Internal_DataPacket(char* data, unsigned short len, unsigned int time, unsigned short id) : Data((char*)NLD_Allocator::NLD_malloc(len)),  TimesSent(0.0f), Size(len), Capacity(len), Id(id), FirstTime(time), LastTime(time) {memcpy(Data, data, len); }
Internal_DataPacket::~Internal_DataPacket(){ NLD_Allocator::NLD_free(Data); }
Internal_DataPacket& Internal_DataPacket::operator=(const Internal_DataPacket& rhs){
	if(Capacity < rhs.Capacity){// we need more space!!
		free();

		Data = (char*)NLD_Allocator::NLD_malloc(rhs.Capacity);// better now.. enough space allocated
	}
	memcpy(Data, rhs.Data, rhs.Size);
	Size = rhs.Size;
	Capacity = rhs.Capacity;
	TimesSent = rhs.TimesSent;
	Id = rhs.Id;
	FirstTime = rhs.FirstTime;
	LastTime = rhs.LastTime;
	return *this;
}
void* cDataPacket::operator new(size_t sz){
	return NLD_Allocator::NLD_malloc((unsigned int)sz);
}
void cDataPacket::operator delete(void* ptr){
	NLD_Allocator::NLD_free(ptr);
}

cDataPacket::cDataPacket(unsigned int len): Internal_Packet(len + sizeof(Network::Internal::ProtocolStandard )){
	Data = Internal_Packet.Data+ sizeof(Network::Internal::ProtocolStandard);// the cDataPacket::Data member is how users can access their buffer. 
	Length = len;
}
cDataPacket::cDataPacket(Internal_DataPacket& obj): Data(0), Length(0){
	memcpy(&Internal_Packet, &obj, sizeof(Internal_DataPacket));// this is perfectly safe as long as I remember to set the other packets data member to 0
	memset(&obj, 0, sizeof(Internal_DataPacket));// this will ensure the packet does not free up the allocated memory...
	Data = Internal_Packet.Data+ sizeof(Network::Internal::ProtocolStandard);// the cDataPacket::Data member is how users can access their buffer. 
	Length = Internal_Packet.Size - sizeof(Network::Internal::ProtocolStandard);
}

cDataPacket &cDataPacket::operator=( const cDataPacket &otherpacket ){
	Internal_Packet = otherpacket.Internal_Packet;
	size_t offset = otherpacket.Data - otherpacket.Internal_Packet.Data;// number of bytes to offset the new buffer. Could be different type of packet with a differnt header
	Data = Internal_Packet.Data + offset;
	Length = otherpacket.Length;
	return *this;
}
void cDataPacket::Init(char* data, unsigned int len){
	unsigned int totalsizeneeded = len + sizeof(Network::Internal::ProtocolStandard);
	if(Internal_Packet.Capacity < totalsizeneeded){
		char* tmp = (char*)NLD_Allocator::NLD_malloc(totalsizeneeded);// enough space to hold the data
		Internal_Packet.Capacity = totalsizeneeded;
		Internal_Packet.free();// delete the old data
		Internal_Packet.Data = tmp;
		// the reliable and unreliable headers are the same size. the data should be offset for the users so where they start writing is the correct location
	} 
	Data = Internal_Packet.Data + sizeof(Network::Internal::ProtocolStandard);// the cDataPacket::Data member is how users can access their buffer. 
	Internal_Packet.Size=totalsizeneeded;
	Length = len;
	memcpy(Data, data, len);	
}
void cDataPacket::reserve(unsigned int newcapacity){
	if(Internal_Packet.Capacity < newcapacity + sizeof(Network::Internal::ProtocolStandard)){
		unsigned int newsize = newcapacity + sizeof(Network::Internal::ProtocolStandard);
		char *data = (char*)NLD_Allocator::NLD_malloc(newsize); // this is the new packets data to be swapped in
		memcpy(data, Internal_Packet.Data, Internal_Packet.Size);// copy all the data in the packet
		Internal_Packet.free();// delete the old data
		Internal_Packet.Data = data;// set the new data array
		Internal_Packet.Capacity= newsize;// set the new capacity
		Data= data + sizeof(Network::Internal::ProtocolStandard);// set the new data pointer
	}
}
void cDataPacket::resize(unsigned int newsize){// resize will not delete the memory if there is already enough allocated
	reserve(newsize);
	Length=newsize;
	Internal_Packet.Size = newsize + sizeof(Network::Internal::ProtocolStandard);
}

void cDataPacket::reservetoblocksize(unsigned int newsize, unsigned int blocksize){
	unsigned int oldsize = newsize;
	unsigned int multiple =  (oldsize) / blocksize;// do not include the header because it doesnt get encrypted. Integer division rounds down
	if( multiple * blocksize !=  (oldsize) ){// round up to the next block size, because the numbers are not equal. 
		reserve( ((multiple +1) *blocksize));
	}
	Length = newsize;
}
void cDataPacket::resizetoblocksize(unsigned int blocksize){
	unsigned int multiple =  (Length) / blocksize;// do not include the header because it doesnt get encrypted. Integer division rounds down
	if( multiple * blocksize !=  (Length) ){// round up to the next block size, because the numbers are not equal. 
		resize( ((multiple +1) *blocksize));
	}
}
unsigned short int cDataPacket::GetID() const{
	return reinterpret_cast<Network::Internal::ProtocolStandard*>(Internal_Packet.Data)->ID;

}
void cDataPacket::SetID(unsigned short int id){
	reinterpret_cast<Network::Internal::ProtocolStandard*>(Internal_Packet.Data)->ID = id;
}