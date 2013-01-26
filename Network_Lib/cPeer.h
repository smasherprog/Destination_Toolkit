#ifndef CHOST_H
#define CHOST_H
#include "cChannel.h"
#include "NetworkUtils.h"
#include <vector>
/*
 *
 * UDP_Engine
 * Copyright (c) 2011 NoLimitsDesigns
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.

 * If there are any concerns or questions about the code, please e-mail smasherprog@gmail.com or visit www.nolimitsdesigns.com
 *

 *
 * Author: Scott Lee
 */
class cDataPacket;
/*!
  cPeer is a class used to hold information about a peer. It can be a connection to a server, or a server. Users should only call a few functions inside the class.
*/
class cPeer{

	bool	I_VerifyInfo(const unsigned int addr, const unsigned short port, const unsigned char sessionid) const ;
	void	I_SetupConnect(unsigned int address, unsigned short port, unsigned short id, unsigned char sessionid, unsigned short mtu, unsigned int receivebandwidth, unsigned int sendbandwidth, unsigned char channelCount);
	
	void	I_ProcessIncomingACKs(unsigned char channel, char* data, unsigned short len, unsigned int recvtime){ IncomingDataTotal+=len; I_Channels[channel].ProcessIncomingACKs(data, len, recvtime); }
	void	I_ProcessIncomingReliable(unsigned char channel, char* data, const unsigned short len, const unsigned int recvtime, const unsigned short id);
	void	I_ProcessIncomingUnReliable(unsigned char channel, char* data, const unsigned short len, const unsigned int recvtime, const unsigned short id);

	void	I_ProcessOutgoingReliable(unsigned char channel, Internal_DataPacket* packet){ OutgoingDataTotal+=packet->Size; I_Channels[channel].ProcessOutgoingReliable(packet); }
	void	I_ProcessOutgoingUnreliable(unsigned char channel, Internal_DataPacket* packet) { }// does nothing Needed to compliment the reliable
	void	I_clear();

	cChannel* I_Channels;
	unsigned int	PingIndex, PacketsLostIndex;
	unsigned int	Pings[ PING_RECORDLENGTH ];
	float			PacketsLost[ PING_RECORDLENGTH ];

public:
	cPeer(): I_Channels(0) { I_clear(); }
	~cPeer(){ I_clear(); }
	
#ifdef THREADING
	std::atomic<unsigned int> 
#else 
	unsigned int 
#endif	
					State;//!< The current state of the client as in PeerState
	unsigned int	Address;//!< this is the destination address where packets are sent to
	unsigned short  DestPort;//!< this is the destination port where packets are sent to
	unsigned short	SrcPort;//!< this is the same as the DestPort if the UDP_Engine is a server. If this is a client, this is the servers sending port
	unsigned short	Id;//!< the current index to ourself in the peer array. If this is a Client, this is the ID in the servers peer array. If this is a server, this is the index to itself within the cPeer array
	unsigned char	SessionId;//!< this is just a random number to help tell when the same computer reconnects
	unsigned short	Mtu; //!< the Maximum Transmission Unit of this peer
	unsigned int	Sendbandwidth;//!< This is the max bytes that will be sent per second to the peer
	unsigned int	Receivebandwidth;//!< This is the max bytes that will be received from the peer --ignored right now
#ifdef THREADING
	std::atomic<unsigned int>
#else 
	unsigned int
#endif
					BytesSentPerSecond,//!< The current amount of data sent. It is reset to zero each time a second passes to regulat bandwitdth
					BytesReceivedPerSecond,//!< The current amount of data received. It is reset to zero each time a second passes to regulat bandwitdth
					NextSentTime, //!< The current amount of time that has passed since the last second. Used to reset BytesSentPerSecond variable to zero
					NextReceiveTime;//!< The current amount of time that has passed since the last second. Used to reset BytesReceivedPerSecond variable to zero
	unsigned char	ChannelCount;//!< Number of open channels on this connection
	unsigned int	IncomingDataTotal;//!< Total amount of data received by this peer
	unsigned int	OutgoingDataTotal;//!< Total amount of data sent by this peer
	unsigned int	NextTimeout;//!< This holds the time that is updated for each packet received from a peer, but a pad is added to it of TIMEOUT. This is used to calc timeouts
	unsigned int	ConnectTime;//!< This is the time that the peer initiated the connection attempt. It is usefull to see how long a peer has been connected
	unsigned int    NextPingTime;//!< This is the next time we will ping the peer
	float			PacketLoss; //!< Current Amount of packet loss as a percent from 0 to 1.
	/*!
		This is the time of the Peer we are connected to. But it is relative to the peer. This would be the unsigned int returned on the connected to machine if it called Network::GetTime()
		This is the time that the peer we are connected to thinks it is. So, if this is a client connected to a server, it would be the servers time, and visa virsa
		Read http://nolimitsdesigns.com/UDP_Engine/html/index.html#syncq for an explination on how this works using ClockTime and TimeClockTime
	*/
	unsigned int	ClockTime; 	
	unsigned int	TimeClockTime;//!<This is the time that the ClockTime was calculated. This is used to in conjunction with ClockTime to figure out when events occred. Read http://nolimitsdesigns.com/UDP_Engine/html/index.html#syncq for an explination on how this works
	unsigned int	EncryptionState;//!< Once a valid keys are echanged, this is set to ENCRYPTION_READY to indicate the peer is able to send and receive encrypted data 
	float			TotalPacketsLost;//!< Total number of packets lost to this Peer
	/*!
		This retrives the average ping using the perameter passed to help 
		\param percent if 10 is passed, the Ping returned will be of the 10% of the best ping times. 100 would mean take all of the packets we use in our Ping History buffer to calculate
	*/
	unsigned int	GetAveragePing( unsigned int  percent);
	float			CalculateLoss(){ PacketLoss = ((TotalPacketsLost/PING_RECORDLENGTH) - 1.0f); return PacketLoss; } //!< this converts the packet loss to a number from 0 meaning no loss, upwards to .5, meaning 50% loss, .6 meaning 60%, etc
	int				GetPeerTime(){ return (GetAveragePing(70)>>1) + Network::GetTime(); }
	/*!
		Used to tranlate a time received from the peer to our time. Since data takes time when traveling over the internet, that time needs to be taken into account.
		\param eventtime should be received from the peer the connection is with and must be from their call to Network::GetTime()
		\return returns converts a peer time to local time
	*/
	
	unsigned int	TranslateToLocalTime(unsigned int eventtime);



#if defined (USEENCRYPTION)
	Network::cEncryptionInfo EncryptionInfo;//!< Containes the AES128 bit symmetric encryption key. When setting up a secure connection, it holds the temp public and private keys, After a conection is made, they are all set to zero
#endif
	friend class UDP_Engine;
	friend class cBaseUDP_Client;
	friend class cBaseUDP_Server;
};

#endif 
