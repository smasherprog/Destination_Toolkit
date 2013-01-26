#ifndef CLOGINCONTROL_H
#define CLOGINCONTROL_H
#include <vector>
#include "../Common_Game_Objects_Lib/cBaseNetwork.h"
#include <string>
#include "../Utilities\NetLib\cUDP_Client.h"
#include "../MY_UI\cWidgetEvents.h"
#include "../MY_UI\TextBox.h"
#include "../MY_UI\Button.h"
#include "../MY_UI\Text.h"
#include "../MY_UI\Button.h"
#include "../MY_UI\Window.h"
#include <memory>
#include "../Graphics_Lib/FXAA.h"

namespace Sound_Engine{
	class Sound;
};

class cLoginControl: public cBaseUDP_Client{
public:

	std::vector<Network::cServerInfo> GameServers;

	cLoginControl();
	virtual ~cLoginControl(){  } 
	unsigned int Focus;

	void DeInit();
	FXAABlender FXAA;
	void Init();
	void Run();

	/////////network functions
	virtual void ReceivePacket(cDataPacket* packet,cPeer* peer, size_t channel);
	virtual void SecureConnectionEstablished(cPeer* peer);
	virtual void Disconnect(cPeer* peer);

	void fServer_Listing(cDataPacket* packet);
	void fLoginto_Gameserver_Success(cDataPacket* packet);

	/////////////////////ui functions
	void Connect();
	void LogintoServer();
	void Disconnect();

	std::shared_ptr<Sound_Engine::Sound> Background_music;

	MY_UI::Controls::TextBox* LoginText, *PasswordText;
	MY_UI::Controls::Button* ConnectButton;
	MY_UI::Controls::Text* ConnectionStatus;

	MY_UI::Controls::Window* ServerSelection;
	std::vector<char> Key;
	unsigned int GameServerIP;
	std::string Login;
};

#endif