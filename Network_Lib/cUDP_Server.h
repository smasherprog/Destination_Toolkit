#ifndef CUDP_SERVER_H
#define CUDP_SERVER_H
#include "UDP_Engine.H"

/*! 
	cUDP_Server has a small bit of framework to start a server. 
*/
class cUDP_Server: public cBaseUDP_Server{
public:

	virtual ~cUDP_Server() {}

	virtual void Connect(cPeer* peer); //!< Write your own code in this function.
	virtual void Disconnect(cPeer* peer);//!< Write your own code in this function.
	virtual void ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel);//!< Write your own code in this function.
	virtual void SecureConnectionEstablished(cPeer* peer);//!< Write your own code in this function.

};


#endif