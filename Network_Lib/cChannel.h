#ifndef CCHANNEL_H
#define CCHANNEL_H
#include <list>
#include "NetworkUtils.h"
#include <vector>
#include "cDataPacket.h"

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
/*!
  This entire class was made private and should never be accessed by anyone. It is completly internal
*/
class cChannel{
	
	cChannel() { clear();}
	~cChannel(){ clear();}
	void clear();

	void	ProcessIncomingACKs(char* data, unsigned int len, unsigned int recvtime);
	void	ProcessIncomingReliable(char* data, const unsigned short len, const unsigned int recvtime, const unsigned short id);
	void	ProcessIncomingUnReliable(char* data, const unsigned short len, const unsigned int recvtime, const unsigned short id);

	void	ProcessOutgoingReliable(Internal_DataPacket* packet);

	void	ProcessReliableFragment(Internal_DataPacket* packet){ ProcessFragment(packet, ReliableFragments, ReliableFragmentCount, ReliableFragmentTotalLength, ReliableFragmentId, ReliableInOrdered);}
	void	ProcessUnReliableFragment(Internal_DataPacket* packet){ ProcessFragment(packet, UnReliableFragments, UnReliableFragmentCount, UnReliableFragmentTotalLength, UnReliableFragmentId, UnReliableIn);}
	void	ProcessFragment(Internal_DataPacket* packet, std::vector<Internal_DataPacket*>& fragments, unsigned int &fragmentcount, unsigned int &fragmenttotalLength, unsigned int &fragmentid, std::list<Internal_DataPacket*>& furtherprocess);

	bool	Pop_ReliableIn(Internal_DataPacket** packet);
	bool	Pop_UnReliableIn(Internal_DataPacket** packet);

	unsigned short GetLowestReliableOutId() const {
		if(ReliableOut.empty())  return ReliableOutId;
		else return ReliableOut.front()->Id;
	}
	unsigned short GetHighestReliableInId() const {
		if(ReliableInUnOrdered.empty()) return ReliableInId;	
		else return (*(--ReliableInUnOrdered.end()))->Id + 1;
	}

	unsigned short IncreaseOutGoingReliableIdBy(unsigned short inc){ ReliableOutId +=inc; return ReliableOutId; }
	unsigned short IncreaseOutGoingUnReliableIdBy(unsigned short inc){ UnReliableOutId += inc; return UnReliableOutId; }

	bool BuildAck();
	void ACKPacket( unsigned int packetID, unsigned int receiveTime );


	char            AckBuffer[ MAXACK_BUFFERLENGTH + sizeof(Network::Internal::ProtocolAcknowledge)];// A buffer of the latest ACK message for this host, plus the size of the header
	unsigned short  AckLength;  // ammount of the buffer actually used.

	unsigned int* PingIndex;
	unsigned int* PacketsLostIndex;

	unsigned int* Pings;// assigned by parent. . . Peer, on creation
	float *PacketsLost, *TotalPacketsLost;// assigned by parent. . . Peer, on creation

	unsigned int	ReliableFragmentCount, ReliableFragmentTotalLength, ReliableFragmentId,// this is an int on purpose so i can initialize it to an invalid packet id
					UnReliableFragmentCount, UnReliableFragmentTotalLength, UnReliableFragmentId;

	std::vector<Internal_DataPacket*> ReliableFragments, UnReliableFragments;
	std::vector<cDataPacket*> Packets;

	std::list<Internal_DataPacket *> ReliableOut;	
	std::list<Internal_DataPacket *> ReliableInOrdered;
	std::list<Internal_DataPacket *> ReliableInUnOrdered;
	std::list<Internal_DataPacket *> UnReliableIn;


#ifdef THREADING
	std::mutex ReliableOutMutex, UnReliableInMutex;
	std::mutex  ReliableInMutex;
	std::atomic<unsigned short> 
#else 	
	unsigned short	
#endif

					ReliableOutId, 
					ReliableInId,
					UnReliableOutId,
					UnReliableInId;
	bool BuildAckPacket;
	public:
	friend class UDP_Engine;
	friend class cPeer;
};

template<class T>void deletelist(T& ls){
	for(T::iterator beg(ls.begin()); beg!= ls.end(); beg++) delete (*beg);	
	ls.clear();
}
#endif