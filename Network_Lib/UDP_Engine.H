#ifndef UDP_Engine_H
#define UDP_Engine_H

#include "cPeer.h"
#include <list>
#include <map>
#include <set>
#include "NetworkUtils.h"
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

/*
	
/*!
  UDP_Engine is a class used as either a client or a server. Check out StartUp for info on how to start either one. The interface is EXTREAMLY basic, all variables labeled with an 
  I_ are interals and should never be used unless you really know what you are doing. 
*/

class UDP_Engine {
protected:
	unsigned short	I_GetIDFor(unsigned int addr, unsigned short port, unsigned char session);
	void			I_ReleaseIDFor(cPeer* peer);

	// encrypted send

	void			I_SendCommand(cPeer* peer, unsigned char channel, unsigned char command);// this is always reliable

	virtual	void	I_ReceiveData()=0;

	virtual bool	I_Locking_Send(cPeer* peer, char* data, unsigned short len)=0;
	virtual bool	I_NonLocking_Send(cPeer* peer, char* data, unsigned short len)=0;
#ifdef THREADING
	void			I_SendTo(Internal_DataPacket& packet, cPeer* peer , unsigned char channel, unsigned char command, std::atomic<unsigned short>& Id, 
						void (cPeer::*process)(unsigned char, Internal_DataPacket*), unsigned short (cChannel::*increaseidby)(unsigned short inc), unsigned char fragcommant );
#else
	void			I_SendTo(Internal_DataPacket& packet, cPeer* peer , unsigned char channel, unsigned char command, unsigned short& Id, 
						void (cPeer::*process)(unsigned char, Internal_DataPacket*), unsigned short (cChannel::*increaseidby)(unsigned short inc), unsigned char fragcommant );
#endif
	void			I_ProcessDisconnect	(cPeer* peer);
	void			I_ProcessConnect	(cPeer* peer);
	void			I_VerifySession(cPeer* peer, Internal_DataPacket* packet);
	bool			I_Decrypt(cPeer* peer, Internal_DataPacket* packet);

	void			I_ReceiveConnect( char *pData, unsigned short len, unsigned int address, unsigned short port, unsigned int receiveTime );
	void			I_ProcessVerifyConnect(cPeer* peer, char *pData, unsigned short len, unsigned int receiveTime );

	void			I_SendPing(cPeer* peer);
	void			I_SendAcknowledge(cChannel* channel, size_t channelindex, cPeer* peer);

	void			I_ResendPackets(cPeer* peer, cChannel* channel);
	void			I_PostProcessing(cPeer* peer, cChannel* channel);
	void			I_PreProcessing(cPeer* peer);
	static bool		I_NetworkRunning();
	static bool		I_Networkstarted;

#ifdef THREADING
	std::mutex	I_ContainersMutex;// ensure mutual explution to the containers
	std::mutex	I_SendLock;// needed to ensure mutuax exclusion to the send method. Data can be interleaved if multiple threads call sendto
#endif
	
	bool			I_Pre_Startup(unsigned short peercount, unsigned short sendbandwidth, unsigned short receivebandwidth, unsigned char numofchannels);

	bool			I_Server, I_Client;
	SOCKET			I_Socket;
	
	std::multimap<unsigned int, unsigned short> I_ConnectedIPs;// destination addresses
	std::set<unsigned short> I_PeerIds;// ensure the same id is not inserted
#if defined(USEENCRYPTION)
	AutoSeededRandomPool rnd;
#endif
	// do not allow this to be copied
	UDP_Engine(const UDP_Engine& obj);
	UDP_Engine& operator=(UDP_Engine& rhs);

public:

	UDP_Engine(): Peers(0), I_Socket(0)  { clear(); };//!< Runs clear() and sets initial values
	virtual ~UDP_Engine(){ clear(); }//!< Runs clear() and destroys everything

	static bool		Initialize_Network();//!< Initialize_Network() sets up all network activitiy. This is automatically called by including the header file cUDP_Engine.h and is called when the object NetworkLibraryStarter is created so the user doesnt have to call it explicitly
	static void		ShutDown_Network();
	//!< ShutDown_Network() shuts down all network activity. This is automatically called by including the header file cUDP_Engine.h and is called when the object NetworkLibraryStarter destroys itself so the user doesnt have to call it explicitly
		/*!
		This is the offial way to start a server or a client. Right now, there can only be one client per UDP_Engine (There is not a slowdown or real speedup from enforcing a one client per UDP_Engine, but I will change this later if enough people request, or I get time). 
		So, to start a server, specificy NULL for the address --thats it. To start a client, make sure to specificy an address and a port. 
		\param pAddress string address representing the destination address to connect to. Specify NULL for a server
		\param port If this is a client startup, the port represents the DESTINATION PORT to send data to. If this is a server, the port represents the LISTENING PORT
		\param peercount is the number of peers that are pre allocated for use. This is fixed and new peers cannot be added beyond this after the server or client is started unless the UDP_Engine is restarted
		\param sendbandwidth the send bandwidth allowed. If this is a server, it is the max bandwidth PER PEER, not the server as a whole. For clients, this is the clients max send rate
		\param receivebandwidth the receive bandwidth allowed. If this is a server, it is the max bandwidth PER PEER, not the server as a whole. For clients, this is the clients max receive rate
		\param numofchannels The total number of channels allowed PER PEER.
		\return if a client is successfully started, the peer is returned. This is really a pointer to the internal cPeer* Peer array so do not try to delete it please. It this is a server, null is returned. Also, if this is a server, run
		ServerRunning() after calling Start to ensure the server had a successful star-up.
	*/
	cPeer* Start(char* pAddress, unsigned short port, unsigned int alternative_addr, unsigned short peercount=1, unsigned short sendbandwidth=UNLIMITEDBANDWIDTH, unsigned short receivebandwidth=UNLIMITEDBANDWIDTH, unsigned char numofchannels=MINCHANNELCOUNT);
	
	void DisconnectPeer(cPeer* peer){ peer->State = Network::PEER_STATE_ACKNOWLEDGING_DISCONNECT; }//!<  this will disconnect the peer the next time Run() is ran
	//ONLY CALL DisconnectPeerNow from code that is not called by the network library. For example, do not call disconnectpeernow if you are in the middle of processing a packet from them and decied you want to disconnec them. Use DisconnectPeer for that, which will remove the peer at the beginning of the next loop. 
	void DisconnectPeerNow(cPeer* peer){ I_ProcessDisconnect(peer); }//!<  this will send a disconnect packet and reset the peer immediately. 
	/*! 
		Run() is the heart of the network engine. For clients, I would not reccomend threading in the UDP_Engine because there would not be enough data, or peers to justify adding threading at that level. However, a server set-up would benfit greatly and can use this as a foundation to build on
		A few virtual functions could be added here and this engine could be incorperated into a larger class to get some per peer threading from the ground up, which would be best for servers.
		For a simple threading library, check out mine at NoLimitsDesigns.com in the downloads section. It is VERY SIMPLE, no need for any extra dlls, libs, or special hoops to jump through; except the fact I built it for Windows(Yickes). 
	*/

	void Run(unsigned short beg, unsigned short end);
	/*! 
		UnreliableSendTo is the fastest of the unreliablesendto calls. It will send the data contained inside the cDataPacket to the peer passed and the channel, BUT IT DOES NOT GUARANTEE THE DATA WILL ACTUALL ARRIVE ON THE OTHER SIDE. The data could be lost in transit, or the bandwidth limit might drop the packet before it is sent out,
		It should be noted that after a call to UnreliableSendTo, the data contained within the packet WILL BE GONE. The UDP_Engine takes ownership of the packet and removes the data inside the packet. This is done for performance reasons and if you want to keep the data, I suggest you make a copy before sending it
		Otherwise, the engine will have to make a copy for each call when most of the time, the data is not needed after the call completes. Also, if the packet is larger than the MTU agreed apon after a succesfull connection
		the packet will be broken up by the engine and sent in peices, then reassembled on the other side. This is done automatically and works for both UnreliableSendTo and ReliableSendTo. Why would you do this on an UnreliableSendTo? I dont know,
		but, I allow it for now. 
		\param packet the packet to send. After a call to UnreliableSendTo, the packet will be empty of data
		\param peer the peer to send the data to
		\param channel the channel to send the packet on. Some users like the idea of having channel 0 as their chat data, channel 1 as their encryption channel and so on. 
	*/
	void UnreliableSendTo(cPeer* peer ,cDataPacket& packet,  unsigned char channel =0){ I_SendTo(packet.Internal_Packet, peer, channel, Network::Internal::COMMAND_UNRELIABLE, peer->I_Channels[channel].UnReliableOutId, &cPeer::I_ProcessOutgoingUnreliable, &cChannel::IncreaseOutGoingUnReliableIdBy, Network::Internal::COMMAND_UNRELIABLE_FRAGMENT); packet.Data =0; packet.Length=0; }
	void MultipleUnreliableSendTo(cPeer* peer ,cDataPacket packet,  unsigned char channel =0){ UnreliableSendTo(peer, packet, channel); }
	// the multiple version will not empty the packet, instead a copy will be made by the library each call. This is usefull when the same data needs to be sent to different clients.
	/*! 
		ReliableSendTo is the fastest of the Reliablesendto calls. It will send the data contained inside the cDataPacket to the peer passed and the channel, AND THE DATA WILL ACTUALL ARRIVE ON THE OTHER SIDE --GUARANTEED, or your money back(Not really!). It should be noted that after a call to ReliableSendTo, the data contained within the packet 
		will be gone. The UDP_Engine takes ownership of the packet and removes the data inside the packet. This is done for performance reasons and if you want to keep the data, I suggest you make a copy before sending it
		Otherwise, the engine will have to make a copy for each call when most of the time, the data is not needed after the call completes. Also, if the packet is larger than the MTU agreed apon after a succesfull connection
		the packet will be broken up by the engine and sent in peices, then reassembled on the other side. This is done automatically and works for both UnreliableSendTo and ReliableSendTo. 
	*/
	void ReliableSendTo	(cPeer* peer, cDataPacket& packet , unsigned char channel =0){ I_SendTo(packet.Internal_Packet, peer, channel, Network::Internal::COMMAND_RELIABLE, peer->I_Channels[channel].ReliableOutId, &cPeer::I_ProcessOutgoingReliable, &cChannel::IncreaseOutGoingReliableIdBy, Network::Internal::COMMAND_RELIABLE_FRAGMENT); packet.Data =0; packet.Length=0; }
	void MultipleReliableSendTo	(cPeer* peer, cDataPacket packet , unsigned char channel =0){ ReliableSendTo(peer, packet, channel); }
	// the multiple version will not empty the packet, instead a copy will be made by the library each call. This is usefull when the same data needs to be sent to different clients.
	/*! 
		ReliableSendTo is the fastest of the Reliablesendto calls. It will send the data contained inside the cDataPacket to the peer passed and the channel, AND THE DATA WILL ACTUALL ARRIVE ON THE OTHER SIDE --GUARANTEED, or your money back(Not really!). It should be noted that after a call to ReliableSendTo, the data contained within the packet 
		will be gone. The UDP_Engine takes ownership of the packet and removes the data inside the packet. This is done for performance reasons and if you want to keep the data, I suggest you make a copy before sending it
		Otherwise, the engine will have to make a copy for each call when most of the time, the data is not needed after the call completes. Also, if the packet is larger than the MTU agreed apon after a succesfull connection
		the packet will be broken up by the engine and sent in peices, then reassembled on the other side. This is done automatically and works for both UnreliableSendTo and ReliableSendTo. 
	*/
	
	void ReliableEncryptedSendTo(cPeer* peer , cDataPacket& packet, unsigned char channel=0);
	void UnreliableEncryptedSendTo(cPeer* peer , cDataPacket& packet, unsigned char channel=0);

	void MultipleReliableEncryptedSendTo(cPeer* peer , cDataPacket packet, unsigned char channel=0){ReliableEncryptedSendTo(peer, packet, channel); }
	void MultipleUnreliableEncryptedSendTo(cPeer* peer , cDataPacket packet, unsigned char channel=0){ UnreliableEncryptedSendTo(peer, packet, channel); }
	// note the function takes a copy of the packet
	// the multiple version will not empty the packet, instead a copy will be made by the library each call. This is usefull when the same data needs to be sent to different clients.
	/*! 
		InitializeEncryption  
		This will initialize encryption between you and the peer. Each peer will generate its own public and private keys. YOU MUST CALL THIS BEFORE ANY CALLS TO sendencrypt. This function sets up the 
		keys and gets everything ready for encryption. Since not all connections need encrpytion and this setup takes a little bit of time. I leave it so this must be called explicitly. Once encryption is ready, 
		the function SecureConnectionEstablished is called from here so the derived application can know to start sending encryption stuff. This is only called once the connection is ready to send and receive encrypted data
	*/
	void InitializeEncryption(cPeer* peer);

	virtual void Connect(cPeer* peer) =0;//!< This is called when ever a connection is established, i.e. after both the server and client have finished handshaking. ALL NEW connections call connect. So, connect can be considered the IN.
	virtual void Disconnect(cPeer* peer) =0;//!< This is called when ever a connection is disconnected. This will be called ONLY for peers that have succesfully connected and called Connect. This is the OUT. ALL PEERS WILL CALL this for each disconnect!
	virtual void ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel) =0;//!< This is called when ever a packet is completely assemlbed and ready to be processed by the application. DO NOT DELETE THE PACKET! The library will take care of it after the function completes. 
	virtual void SecureConnectionEstablished(cPeer* peer) =0;//!<This is called after a call to InitializeEncryption has been succesfull. After both sides exchange keys, this function is called so that a derived application can know to start doing encryption stuff

	bool ServerRunning(){ return I_Server; }//!< Simple function to check and see if this is a server, or a client. If the server started up succesfully, use this to verify after a call to StartUp
	bool ClientRunning(){ return I_Client; }//!< Simple function to check and see if this is a server, or a client. If the client started up succesfully, use this to verify after a call to StartUp

	void clear();//!< Calling clear is not threadsafe and once it is done, all connections and peers will be reset. This will effectivly shut everything down and reset it.
	/*! The Peers pointer is to an array created on a call to StartUp(). It is created once per call to startup, and is not dynamic, meaning if you
		call startup with 50 peers allowed, that will be the limit until you call startup again (which would reset your server, or client).
	*/
	cPeer			*Peers; 
	unsigned int    BytesRecieved, //!< The total number of bytes received by this Engine since a call to StartUp, this includes all clients if this is a server
					BytesSent,//!< The total number of bytes send by this Engine since a call to StartUp, this includes all clients if this is a server
					PacketsSent, //!< The total number of packets send by this Engine since a call to StartUp, this includes all clients if this is a server
					PacketsReceived,//!< The total number of received send by this Engine since a call to StartUp, this includes all clients if this is a server
					PacketsLost, //!< The total number of packets lost by this Engine since a call to StartUp, this includes all clients if this is a server
					PerPeerSendBandwidth, //!< This is the maximim allowed bytes per second that will be sent out from this connection. This includes all clients if this is a server
					PerPeerReceiveBandwidth;//!< This is the maximim allowed bytes per second that will be recevied out from this connection. This includes all clients if this is a server
#ifdef THREADING
	std::atomic<unsigned short>
#else
	unsigned short
#endif
					NumOfPeers; //!< The total number of peers current connected to this if it is a server, or if this is a client, it should be one

	unsigned short	MaxMTU, //!< The largest packet size allowed to be sent or received by this connection. Anything larger than this being send out will be fragmented.
					MaxNumOfPeers;//!< The total number of peers allowed connected to this if it is a server, or if this is a client, it should be one
	unsigned char	MaxChannelCount;//!< The total number of channels allowed connected to this if it is a server, 


};
/*! 
	This is a middle man. It was created because of the different way a server and client handle sending and receiving data. I did not want to clutter the derived class with this and it is internal stuff.
	The virtual functions are all known at compile time, therefore, the compiler SHOULD be treating these functions as standard functions. . . . 
*/
class cBaseUDP_Server: public UDP_Engine{
protected:
	virtual	void	I_ReceiveData();
	virtual bool	I_Locking_Send(cPeer* peer, char* data, unsigned short len);
	virtual bool	I_NonLocking_Send(cPeer* peer, char* data, unsigned short len);
	/*! 
		In a typical server setup, you authenticate users through a login and password or some other type of authentication. 
		However, before this occurs, there is a connection between the end points. You do not want peers to connect, 
		then sit around and not authenticate themselves, as this can use up resources. So, a server should kick those who have not authenticated themsevles after a certain time period.
		This list holds those users who the server has not yet authenticated.
	*/
	std::set<cPeer*> I_UnAuthenticatedPeers_;
public:
	virtual ~cBaseUDP_Server() {}
	/*! 
		Servers only listen. they do not connect. Once called, the server will be listening on the specified port.
		\param port the desired port to listen on
		\param numofpeers the Maximum number of peers this server will allow. This cannot change once set unless the server is restarted
		\param sendbandwidth the value is clamped from MINPEERBANDWIDTH to a max of UNLIMITEDBANDWIDTH
		\param receivebandwidth the value is clamped from  MINPEERBANDWIDTH to a max of UNLIMITEDBANDWIDTH
		\param numofchannels the value is clamped from  MINCHANNELCOUNT to a max of MAXCHANNELCOUNT
		
	*/
	bool Listen(unsigned short port, unsigned short numofpeers = 16, unsigned short sendbandwidth=UNLIMITEDBANDWIDTH, unsigned short receivebandwidth=UNLIMITEDBANDWIDTH, unsigned char numofchannels=MINCHANNELCOUNT);		
	void Run();//!< The main loop for check for data, ping users, kick unauthenticated users after a certain time has passed
	void DisconnectAll();//!< This will disconnect all users right now and clear them
	void AuthenticatePeer(cPeer* peer);//!< this just removes the peer from a watched list of unauthorized peers. Call this after the peer has provieded valid login and password information
	/*!
		if a peer is reinserted into this list, the peer will be removed after the timelimit expires to authenticate. You can place peers here if you wish them to 
		disconnect later. If you want to disconnect the peers later, you can set the cPeer::ConnectTime to set the exact time when the peer will be removed. 
		The checks are done in cBaseUDP_Server::Run  Check that to see how its possible.
		This example is how you can ensure that a peer is removed later on. Sometimes, you want to send the peer some information, and then disconnect. But, you would need to have a list of peers in a waiting state. 
		You already have that now!! So use this if you want to ensure the peer is disconnected at a later time
		UnAuthenticatePeer(peer);
		peer->ConnectTime = Network::GetTime() + TIMEOUT;// the current time, plus TIMEOUT ms. The peer will be disconnected the peer after TIMEOUT forcibly
	*/
	void UnAuthenticatePeer(cPeer* peer);
	virtual void Connect(cPeer* peer){ I_UnAuthenticatedPeers_.insert(peer); }//!< This is not required to be called in a derived class unless you want to use the authentication system in place. If this is called, Disconnect must be called also when a peer disconnects.  The function inserts this peer into the set. If the user does not authenticate themself within the time of AUTHENTICATIONCHECKTIMEOUT, Disconnect the peer
	virtual void Disconnect(cPeer* peer){ I_UnAuthenticatedPeers_.erase(peer);}//!< This is not required to be called in a derived class unless you want to use the authentication system in place. If this is called, Connect should have been called also when a peer connected.  This function removes the peer from the list of peers that we are waiting to authenticate
	

};
/*! 
	This is a middle man. It was created because of the different way a server and client handle sending and receiving data. I did not want to clutter the derived class with this and it is internal stuff.
	The virtual functions are all known at compile time, therefore, the compiler SHOULD be treating these functions as standard functions. . . . 
*/
class cBaseUDP_Client: public UDP_Engine{
protected:
	virtual	void	I_ReceiveData();
	virtual bool	I_Locking_Send(cPeer* peer, char* data, unsigned short len);
	virtual bool	I_NonLocking_Send(cPeer* peer, char* data, unsigned short len);
public:

	cBaseUDP_Client(): Peer(0) {}
	virtual ~cBaseUDP_Client(){}
	cPeer* Peer;
	 
	/*! 
		Clients only connet to a destination address and port. 
		\param pAddress connects to the destination address. Valid addesses are "www.google.com", or any other www. address. Or an ip address, such as "207.34.23.103". All as strings
		\param port the desired port to send to
		\param sendbandwidth the value is clamped from MINPEERBANDWIDTH to a max of UNLIMITEDBANDWIDTH
		\param receivebandwidth the value is clamped from  MINPEERBANDWIDTH to a max of UNLIMITEDBANDWIDTH
		\param numofchannels the value is clamped from  MINCHANNELCOUNT to a max of MAXCHANNELCOUNT
		
	*/
	bool ConnectTo(char* pAddress, unsigned short port, unsigned short sendbandwidth=UNLIMITEDBANDWIDTH, unsigned short receivebandwidth=UNLIMITEDBANDWIDTH, unsigned char numofchannels=MINCHANNELCOUNT);
	bool ConnectTo(unsigned int pAddress, unsigned short port, unsigned short sendbandwidth=UNLIMITEDBANDWIDTH, unsigned short receivebandwidth=UNLIMITEDBANDWIDTH, unsigned char numofchannels=MINCHANNELCOUNT);
	void Run();
	/*! Read ReliableSendTo for info on this function. This is just a wrapper */
	template<class T>void ReliableSendTo(unsigned short int header_id, T& datatosend, unsigned char channel =0){ 
		cDataPacket pac((char*)&datatosend, sizeof(T));
		pac.SetID(header_id);
		UDP_Engine::ReliableSendTo(Peer, pac, channel);	
	}
	/*! Read UnreliableSendTo for info on this function. This is just a wrapper */
	template<class T>void UnreliableSendTo(unsigned short int header_id,  T& datatosend, unsigned char channel =0){ 
		cDataPacket pac((char*)&datatosend, sizeof(T));
		pac.SetID(header_id);
		UDP_Engine::UnreliableSendTo(Peer, pac, channel);	
	}
	/*! Read UnreliableSendTo for info on this function. This is just a wrapper */
	void UnreliableSendTo(cDataPacket& packet, unsigned char channel =0){ UDP_Engine::UnreliableSendTo(Peer, packet,  channel); }
	/*! Read ReliableSendTo for info on this function. This is just a wrapper */
	void ReliableSendTo	(cDataPacket& packet, unsigned char channel =0){ UDP_Engine::ReliableSendTo(Peer, packet,  channel); }
	/*! Read UnreliableSendTo for info on this function. This is just a wrapper */
	void UnreliableSendTo(unsigned short int header_id, unsigned char channel =0){ 
		cDataPacket pac(0);
		pac.SetID(header_id);
		UDP_Engine::UnreliableSendTo(Peer, pac, channel);	
	}
	/*! Read ReliableSendTo for info on this function. This is just a wrapper */
	void ReliableSendTo	(unsigned short int header_id, unsigned char channel =0){ 
		cDataPacket pac(0);
		pac.SetID(header_id);
		UDP_Engine::ReliableSendTo(Peer, pac, channel);	
	}

	/*! Read UnreliableSendTo for info on this function. This is just a wrapper */
	void UnreliableSendTo(unsigned short int header_id, char* datatosend, unsigned int bytes, unsigned char channel =0) { 
		cDataPacket pac(datatosend, bytes);
		pac.SetID(header_id);
		UDP_Engine::UnreliableSendTo(Peer, pac, channel);	
	}
	/*! Read ReliableSendTo for info on this function. This is just a wrapper */
	void ReliableSendTo(unsigned short int header_id, char* datatosend, unsigned int bytes, unsigned char channel =0) { 
		cDataPacket pac(datatosend, bytes);
		pac.SetID(header_id);
		UDP_Engine::ReliableSendTo(Peer, pac, channel);
	}
	/*! Read ReliableEncryptedSendTo for info on this function. This is just a wrapper */
	void ReliableEncryptedSendTo( cDataPacket& packet, unsigned char channel =0){ UDP_Engine::ReliableEncryptedSendTo(Peer,packet,  channel); }
	/*! Read UnreliableEncryptedSendTo for info on this function. This is just a wrapper */
	void UnReliableEncryptedSendTo( cDataPacket& packet, unsigned char channel =0){UDP_Engine::UnreliableEncryptedSendTo(Peer, packet,  channel); }	
	/*! Read ReliableEncryptedSendTo for info on this function. This is just a wrapper */
	template<class T>void ReliableEncryptedSendTo(unsigned short int header_id, T& datatosend, unsigned char channel =0){ 
		cDataPacket pac(&datatosend, sizeof(T));
		pac.SetID(header_id);
		UDP_Engine::ReliableEncryptedSendTo(Peer, datatosend, channel);
	}
	/*! Read UnreliableEncryptedSendTo for info on this function. This is just a wrapper */
	template<class T>void UnreliableEncryptedSendTo(unsigned short int header_id, T& datatosend, unsigned char channel =0){ 
		cDataPacket pac(&datatosend, sizeof(T));
		pac.SetID(header_id);
		UDP_Engine::UnreliableEncryptedSendTo(Peer, pac, channel);
	}
	/*! Read UnReliableEncryptedSendTo for info on this function. This is just a wrapper */
	void UnreliableEncryptedSendTo(unsigned short int header_id, char* datatosend, unsigned int  bytes,unsigned char channel =0){ 
		cDataPacket pac(datatosend, bytes);
		pac.SetID(header_id);
		UDP_Engine::UnreliableEncryptedSendTo(Peer, pac, channel);
	}
	/*! Read ReliableEncryptedSendTo for info on this function. This is just a wrapper */
	void ReliableEncryptedSendTo(unsigned short int header_id, char* datatosend, unsigned int  bytes, unsigned char channel =0){
		cDataPacket pac(datatosend, bytes);
		pac.SetID(header_id);
		UDP_Engine::ReliableEncryptedSendTo(Peer, pac, channel);
	}
	/*!  Read UDP_Engine::DisconnectPeer for info on this function. This is just a wrapper */
	void DisconnectPeer(){if(Peer) UDP_Engine::DisconnectPeer(Peer);}
	/*! Read UDP_Engine::DisconnectPeerNow for info on this function. This is just a wrapper. This function should only be called if you really are disconnecting right now. */
	void DisconnectNow(){ if(Peer) UDP_Engine::DisconnectPeerNow(Peer); Peer=0;}

	virtual void Connect(cPeer* peer) { InitializeEncryption(peer); }//!< This function can be ignore if you want to set up encryption at a different time, or not at all. In that case, forgoe calling this in your own derived Connect Call.
	virtual void Disconnect(cPeer* peer) { Peer =0; }//!< This function should ALWAYS be called if it is defined in a derived class

};

class NetworkLibraryStarter{//delcare this class in main to startup and shut down things 
public:
	NetworkLibraryStarter(){ UDP_Engine::Initialize_Network(); }
	~NetworkLibraryStarter(){ UDP_Engine::ShutDown_Network(); }
};	

#endif 
