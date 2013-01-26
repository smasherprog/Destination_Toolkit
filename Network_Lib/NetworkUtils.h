#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H
#include <string>
#include <iostream>
#include <atomic>
#include <mutex>

#define THREADING 1
// the following defines are here for those who do not want to use threading building blocks or the encryption library and want a simple build...
#define USEENCRYPTION 1// comment this out if you do not want to use the encryption library

#if defined (USEENCRYPTION)
	#pragma comment(lib, "Crypto++_Lib")
	#include "../crypto++/integer.h"
	using CryptoPP::Integer;
	#include "../crypto++/osrng.h"
	using CryptoPP::AutoSeededRandomPool;
	#include "../crypto++/fhmqv.h"
	#include "../crypto++/eccrypto.h"
	using CryptoPP::ECP;
	using CryptoPP::FHMQV;
	#include "../crypto++/dh.h"
	using CryptoPP::DH;
	#include "../crypto++/dh2.h"
	using CryptoPP::DH2;
	#include "../crypto++/secblock.h"
	using CryptoPP::SecByteBlock;
	#include "../crypto++/aes.h"
	using CryptoPP::AES;
	#include "../crypto++/sha.h"
	using CryptoPP::SHA256;
	#include "../crypto++/gcm.h"
	using CryptoPP::GCM;
	#include "../crypto++/asn.h"
	#include "../crypto++/oids.h"
	using CryptoPP::OID;
	using namespace CryptoPP::ASN1;

#endif


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
	enable TESTNETWORK to preform network testing. The settings below will make the network library simulate packet loss and or a set ping.
	The packet loss setting will actually randomly drop packets when they are received, effectivly simulating real packet loss.
	The ping setting will hold packets and not send until the time has passed/ Be aware, there is a very small speed penalty for this because of a few extra calculations 
	that occur. So, if you dont need to test the network, then it is best to uncomment TESTNETWORK.
*/
//#define TESTNETWORK
#if defined(TESTNETWORK)
	#define TARGETPACKETLOSS 1   //!< expressed as a percent, 10=10%, 30 = 30% packet loss. A setting of 1 would turn the simulated packet loss off and the only packet loss would be REAL packet loss
	/*!
		PACKETLOSSRANGE 
		this will fluctuate the packet loss up and down to keep it from being uniform. For example, a setting of 10 for TARGETPACKETLOSS and 10 for PACKETLOSSRANGE will mean the packet loss will range between
		5% and 15%    A setting of 10 for PACKETLOSSRANGE means a packet loss range of +-5%
	*/
	#define PACKETLOSSRANGE 1
	#if PACKETLOSSRANGE > TARGETPACKETLOSS // prevent a packet loss range exceeding the targetpacket loss, which doesnt make sense
		#undef PACKETLOSSRANGE
		#define PACKETLOSSRANGE TARGETPACKETLOSS
	#endif
	#if PACKETLOSSRANGE == 0  // prevent a packet loss range from being zero and causing an error to occur
		#undef PACKETLOSSRANGE
		#define PACKETLOSSRANGE 1
	#endif
	#if TARGETPACKETLOSS == 0  // prevent target packet loss range from being zero and causing a divide by zero to occur
		#undef TARGETPACKETLOSS
		#define TARGETPACKETLOSS 1
	#endif
	#define TARGETPING 200   //!< expressed in miliseconds. A setting of 1 will turn this off and send packets as soon as possible
	#define PINGRANGE 100  //!< This has a simuliar effect as PACKETLOSSRANGE. A setting of 100 for this means the ping will fluctuate +-50ms
	#if PINGRANGE > TARGETPING // prevent a ping range exceeding the targetping, which doesnt make sense
		#undef PINGRANGE
		#define PINGRANGE TARGETPING
	#endif

#endif
//#define NETWORKDEBUG//!< uncomment this if you want to see all of my debug text output to the console

#if defined(NETWORKDEBUG) 
	#ifndef OUTPUT_NETWORK_DEBUG_MSG // nothing special about this, I use the console for standard debug info
	#define OUTPUT_NETWORK_DEBUG_MSG(x) {												\
		std::cout<<x<<std::endl;													     	\
	}
	#endif
#else
	#ifndef OUTPUT_NETWORK_DEBUG_MSG
	#define OUTPUT_NETWORK_DEBUG_MSG(x) {}
	#endif
#endif 

template<class T>T clamp(T X, T Min, T Max) {
   return ( X > Max ) ? Max : ( X < Min ) ? Min : X;
}	


#define MINCHANNELCOUNT			1 //!< Min number of channels allowed
#define MINPEERCOUNT			1//!< Min number of peers allowed
#define MAXCHANNELCOUNT			31 //!< Max number of channels +1. This is includes 0, so 31 is 32. Channels [0, MAXCHANNELCOUNT]
#define MINPEERBANDWIDTH		5000 //!< The minimum Per Peer bandwidth 
#define UNLIMITEDBANDWIDTH		0xffff //!< Unlimited bandwidth!!
#define MINMTU					576//!<the tcp/ip standard guarantees the min MTU to be a 576 bytes. However, most users should be able to send larger packets
#define DEFAULTMTU				1440 //!< this is a good value to use as a default. Check out http://en.wikipedia.org/wiki/Maximum_transmission_unit for more information 
#define MAXMTU					4096   //!< most ethernets only allowed a 1500 size frame (or packet), but I figured, what the heck the IP layer will fragment the packet if it is higher than the Link Layers MTU
#define MAXPEERCOUNT			4096   //!< the maximum number of peers allowed. This is the highest ID number allowed as well
#define MAXNUMOFFRAGMENTS		1000   //!< this is the maximum number of peices that a single packet will be broken up into if it is over the MTU agreed apon by the hosts
#define MAXSESSIONID			0x0007 //!< Session ids go from 0 to 8. It just adds extra help to differentiate connections from the same computer.
#define ACK_MAXPERMSG           256 //!< Max number of ack messages allowed per Ack packet. This should work fine in most cases
#define MINACK_BUFFERLENGTH		sizeof(unsigned int) + sizeof(char) //!< Minimim size of an ack packet
#define MAXACK_BUFFERLENGTH		sizeof(unsigned int) + sizeof(char) + (ACK_MAXPERMSG / 8) //!< Maximum size of an ack packet
#define PING_RECORDLENGTH       64  //!< the window, or number of reliable packets used to calculate the ping and packet loss. Lager number means more packets are taken into account to calculate the ping, too long doesnt give accurate results
#define PING_DEFAULT			300  //!<Time in milisecords that is setup before a connection is established for a default ping value. If a lower number is used, the client or server will hammer the other peer trying to connect. This is basically the default ping until we send and receive enough packets to calculate one
#define TIMEOUT					10000 //!<timeout in miliseconds before a client or server will terminate a connection with a peer after not hearing from them.
#define PINGRATE				500   //!< ping users every 500 ms, which is every half a second
#define PACKETLOSSDISCONNECT   .40f //!< this is the percent a users packet loss can goto before you disconect from them. Users with high packet loss can be a stress on a connection because of the resending

namespace Network{ //this is used to hide most of the functions from the user, so he or she will not see a jumble of functions and member variable in the classes. 

	class Header_ID{
		Header_ID(){};// do not allow empty constructor
	public:
		explicit Header_ID(unsigned short int id) : ID(id) {}// do not allow the compiler to allow assignment
		unsigned short int ID;
	};
	unsigned int GetTime();

#if defined(USEENCRYPTION)
	class cEncryptionInfo{
	public:
		cEncryptionInfo();
		~cEncryptionInfo();
		void clear();
		void clear_keyexchange();
		void Init(bool client);
		FHMQV < ECP >::Domain* fhmqv;
		SecByteBlock staticprivatekey, staticpublickey, ephemeralprivatekey, ephemeralpublickey, AESKey;
	};
#endif
	/*! 
	In the cPeer class, there is a variable called State, which correspond to one of the values in the enum. So, if you want to know what the status of a peer is here is how
	*/
	enum PeerState{
		PEER_STATE_DISCONNECTED,              
		PEER_STATE_CONNECTING,               
		PEER_STATE_ACKNOWLEDGING_CONNECT,       
		PEER_STATE_CONNECTED,                   
		PEER_STATE_ACKNOWLEDGING_DISCONNECT,    
		PEER_STATE_ENCRYPTION_NOT_READY,       
		PEER_STATE_EXCHANGING_KEYS,           
		PEER_STATE_ENCRYPTION_READY,           
	};
	const std::string Peer_State[] = {
		"DISCONNECTED",              
		"CONNECTING",               
		"ACKNOWLEDGING_CONNECT",       
		"CONNECTED",                   
		"ACKNOWLEDGING_DISCONNECT",    
		"ENCRYPTION_NOT_READY",       
		"EXCHANGING_KEYS",           
		"ENCRYPTION_READY"    
	};
	namespace Internal{
		enum Commands {		    
		   COMMAND_CONNECT,
		   COMMAND_VERIFY_CONNECT,

		   COMMAND_ACKNOWLEDGE,
		   COMMAND_DISCONNECT,  
		   COMMAND_UNRELIABLE_FRAGMENT,
		   COMMAND_ENCRYPTED_UNRELIABLE_PACKET,
		   COMMAND_UNRELIABLE,     

		   COMMAND_PING,   
		   COMMAND_RELIABLE_FRAGMENT,
		   COMMAND_INIT_ENCRYPTION,
		   COMMAND_ENCRYPTED_RELIABLE_PACKET,
		   COMMAND_RELIABLE,          
		   NUMOFCOMMANDS
		};
		const std::string CommandsToString[NUMOFCOMMANDS+1]={
			"COMMAND_CONNECT",
			"COMMAND_VERIFY_CONNECT", 
			 
			"COMMAND_ACKNOWLEDGE",
			"COMMAND_DISCONNECT",  
			"COMMAND_UNRELIABLE_FRAGMENT", 
			"COMMAND_ENCRYPTED_UNRELIABLE_PACKET",
			"COMMAND_UNRELIABLE",  

			"COMMAND_PING",    
			"COMMAND_RELIABLE_FRAGMENT",
			"COMMAND_INIT_ENCRYPTION",
			"COMMAND_ENCRYPTED_RELIABLE_PACKET",
			"COMMAND_RELIABLE",          
			};
		#if defined(_MSC_VER)
			#pragma pack(push, 1)// this ensures that all of the structures below are aligned to within 1 byte (packed tight). 
			#define UDP_PACKED
		#elif defined(__GNUC__)
			#define UDP_PACKED __attribute__ ((packed))
		#else 
			#define UDP_PACKED
		#endif
		UDP_PACKED struct ProtocolCommandHeader{
			unsigned short SequenceNumber;
			unsigned char command, ChannelID;//<! the first four bytes of the channel is used to store information when encryption is occuring so do not mess with it. 
			unsigned short PeerID;
		};
		UDP_PACKED struct ProtocolStandard{
			ProtocolCommandHeader header;
			unsigned short ID;// this is new and basically, every packet that is going to be made with a real payload, should have an id for the application layer. This is where that ID resides.
		};
		UDP_PACKED struct ProtocolAcknowledge{
			ProtocolCommandHeader header;
			unsigned int SentTime;
		};
		UDP_PACKED struct ProtocolConnect{
		   ProtocolCommandHeader header;
		   unsigned short mtu;
		   unsigned int receivebandwidth, sendbandwidth;
		   unsigned char channelCount;
		};
		UDP_PACKED struct ProtocolFragment{
		   ProtocolCommandHeader header;
		   unsigned short startSequenceNumber;// the starting number of the packet this fragment belongs to
		   unsigned short fragmentCount;// how many fragments there are 
		   unsigned int totallength;
		};
		#if defined(_MSC_VER)
			#pragma pack(pop)
		#endif

	};
};

#endif