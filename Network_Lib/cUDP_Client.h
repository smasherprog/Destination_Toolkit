#ifndef CUDP_CLIENT_H
#define CUDP_CLIENT_H
#include "UDP_Engine.H"
#include <functional>

/*!
	cUDP_Client is the framework for a client to connect to a cUDP_Server. Follow it in building applications. Most clients will not need to change the connect and disconnect functions. 

*/
class cUDP_Client: public cBaseUDP_Client{
public:

	static std::function<void (char*, unsigned int, size_t)> Empty_Function;// There should not be any holes in the array of functions. They should ALL be contiguous, otherwise you are not progamming this correctly.
	//If for some reason you have a gap, you should plug the gap with a function that deals with a packet that comes in on that id. This is just an example of a function to place in empty holes, however, you could add a function to log people who probe your network code
	std::vector<std::function<void (char*, unsigned int, size_t channel)>> Net_Functions;

	virtual ~cUDP_Client(){}//!< this is needed to ensure proper cleanup occurs 
	virtual void Connect(cPeer* peer);//!< Write your own code in this function. Read cUDP_Engine::Connect and cBaseUDP_Client::Connect for a better discription.
	virtual void Disconnect(cPeer* peer);//!< Write your own code in this function. Read cUDP_Engine::Disconnect and cBaseUDP_Client::Disconnect for a better discription.
	virtual void ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel);//!< Write your own code in this function. Read cUDP_Engine::ReceivePacket for a better discription.
	virtual void SecureConnectionEstablished(cPeer* peer);//!< Write your own code in this function

	virtual void Prep_Function_Buffer(unsigned short int maxfunction_index);// call this before calling register function so there are no reoccuring allocations
	virtual void Register_Function(std::function<void (char*, unsigned int, size_t channel)> func, unsigned short int index);
};


#endif