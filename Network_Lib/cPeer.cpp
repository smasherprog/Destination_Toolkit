#include "stdafx.h"
#include "cPeer.h"
#include "cDataPacket.h"

void cPeer::I_clear(){
	EncryptionState=Network::PEER_STATE_ENCRYPTION_NOT_READY;
	State = Network::PEER_STATE_DISCONNECTED;
	Address= 0;
	DestPort=0;
	Id= 0;
	SrcPort=0;
	SessionId= 0;
	BytesSentPerSecond=0;
	BytesReceivedPerSecond=0;
	NextSentTime=0;
	NextReceiveTime=0;
	ClockTime=0;
	Mtu= DEFAULTMTU;
	Sendbandwidth = MINPEERBANDWIDTH;
	Receivebandwidth = MINPEERBANDWIDTH;
	ChannelCount = 0;
	if(I_Channels){ 
		delete [] I_Channels;
		I_Channels =0;
	}
	IncomingDataTotal=0;
	OutgoingDataTotal=0;
	ClockTime = 0;
	NextTimeout=0;
	PacketLoss=0;  
	NextPingTime=0;
	for( unsigned int i = 0; i < PING_RECORDLENGTH; i++ ){
		Pings[ i ] = PING_DEFAULT;
		PacketsLost[i] =1.0f;
	}
	PingIndex=PacketsLostIndex=0;
	TotalPacketsLost = PING_RECORDLENGTH;// this is the window size
}

bool cPeer::I_VerifyInfo(const unsigned int addr, const unsigned short port, const unsigned char sessionid) const { 
#if defined(DEBUG)
	if(!((Address == addr) & (SrcPort == port) & (sessionid == SessionId))){
		if(Address != addr) OUTPUT_NETWORK_DEBUG_MSG("Addresses do not match: "<<Address<<" != "<<addr);
		if(SrcPort != port) OUTPUT_NETWORK_DEBUG_MSG("Ports do not match: "<<SrcPort<<" != "<<port);
		if(SessionId != sessionid) OUTPUT_NETWORK_DEBUG_MSG("sessionids do not match: "<<(unsigned int)SessionId<<" != "<<(unsigned int)sessionid);
		return false;
	}
	return true;
#else 
	return (Address == addr) & (SrcPort == port) & (sessionid == SessionId);
#endif
}
static int sSortPing( const void *arg1, const void *arg2 ) {
  if( *(unsigned int *)arg1 < *(unsigned int *)arg2 ) return -1;
  if( *(unsigned int *)arg1 > *(unsigned int *)arg2 ) return 1;
  return 0;
}
unsigned int cPeer::GetAveragePing( unsigned int bestPercentage){
	if( (bestPercentage <= 0) | (bestPercentage > 100) )
		bestPercentage = 100;
	unsigned int pings[ PING_RECORDLENGTH ];
	// Recalculate the ping list

	memcpy( pings, Pings, PING_RECORDLENGTH * sizeof( unsigned int ) );
	qsort( pings, PING_RECORDLENGTH, sizeof( unsigned int ), sSortPing );
	unsigned int worst = PING_RECORDLENGTH * bestPercentage / 100;
	unsigned int sum = 0;
	for(unsigned int i = 0; i < worst; i++ )
		sum += pings[ i ];
	return sum / worst;
}

void cPeer::I_ProcessIncomingReliable(unsigned char channel, char* data, const unsigned short len, const unsigned int recvtime, const unsigned short id){
	IncomingDataTotal+=len;
	I_Channels[channel].ProcessIncomingReliable(data, len, recvtime, id);
}
void cPeer::I_ProcessIncomingUnReliable(unsigned char channel, char* data, const unsigned short len, const unsigned int recvtime, const unsigned short id){
	IncomingDataTotal+=len; 
	I_Channels[channel].ProcessIncomingUnReliable(data, len, recvtime, id);
}

void cPeer::I_SetupConnect(unsigned int address, unsigned short port, unsigned short id, unsigned char sessionid, unsigned short mtu, unsigned int receivebandwidth, unsigned int sendbandwidth, unsigned char channelCount){
	I_clear();
	// now we have a valid slot in id 
	State = Network::PEER_STATE_CONNECTING;
	Address= address;
	DestPort=port;
	Id= id;
	SessionId= sessionid;
	Mtu= mtu;
	Receivebandwidth = receivebandwidth;
	Sendbandwidth = sendbandwidth;
	ChannelCount = channelCount;
	ConnectTime = Network::GetTime();
	NextTimeout = ConnectTime + TIMEOUT;
	NextPingTime = ConnectTime + PINGRATE;
	I_Channels = new cChannel[channelCount];// allocate channels
	for(unsigned char i(0); i<channelCount; i++){
		I_Channels[i].Pings = Pings;
		I_Channels[i].PacketsLost = PacketsLost;
		I_Channels[i].PingIndex = &PingIndex;
		I_Channels[i].PacketsLostIndex = &PacketsLostIndex;
		I_Channels[i].TotalPacketsLost = &TotalPacketsLost;
	}
}
unsigned int cPeer::TranslateToLocalTime(unsigned int eventtime) {// eventtime should be a timestamp received from the connected peer
	unsigned int time = Network::GetTime();
	unsigned int mytimedifference = time - TimeClockTime;// this is the time that has passed since ClockTime was calculated. It will always be positive, time doesnt run backwards yet!!
	unsigned int newservertime = ClockTime + mytimedifference;// this is the updated time of the server. This is relative to itself, so the value doesnt make sense to us except to show how much time has passed. 
	return time + (eventtime - newservertime);
}