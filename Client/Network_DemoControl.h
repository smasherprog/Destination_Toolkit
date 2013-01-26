#ifndef NETWORK_DEMOCONROL_H
#define NETWORK_DEMOCONROL_H
#include "../Utilities/NetLib/UDP_Engine.h"
#include <functional>
#include <vector>

struct CharacterChange;

class Network_DemoControl: public cBaseUDP_Client {
public:

	Network_DemoControl() {}
	~Network_DemoControl(){ DeInit();}

	static std::function<void (char*, unsigned int, size_t)> Empty_Function;
	//If for some reason you have a gap, you should plug the gap with a function that deals with a packet that comes in on that id. This is just an example of a function to place in empty holes, however, you could add a function to log people who probe your network code
	std::vector<std::function<void (char*, unsigned int, size_t)>> Net_Functions;

	void Prep_Function_Buffer(unsigned short int maxfunction_index);
	void Register_Function(std::function<void (char*, unsigned int, size_t channel)> func, unsigned short int index);
	void Dispatch(unsigned short int index, char* data, unsigned int length, size_t channel);
	void Init(uint32_t gameserverip, const std::string& login, const uint8_t* key);
	void DeInit();
	void Run();
	// must override these
	virtual void ReceivePacket(cDataPacket* packet, cPeer* peer, size_t channel) override;
	virtual void SecureConnectionEstablished(cPeer* peer) override;
	virtual void Disconnect(cPeer* peer) override;

	void ConnectToGameServer();

	std::string Login;
	char Key[SHA256::DIGESTSIZE];

};



#endif