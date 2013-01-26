#include "stdafx.h"
#include "cLoginControl.h"
#include <string>
#include "../Graphics_Lib\Globals_GraphicsCore.h"
#include "../Sound_Lib\SoundEngine.h"
#include "UI_LoginControl.h"
#include "Global_GameData.h"


cLoginControl::cLoginControl(){
	LoginText=PasswordText=0;
	ConnectButton=0;
	ConnectionStatus=0;
	ServerSelection=0;
	Init();
}

void cLoginControl::DeInit(){
	OUTPUT_DEBUG_MSG("Deinit on login server");
	DisconnectPeer();
	MY_UI::Safe_Delete(ConnectButton);
	MY_UI::Safe_Delete(LoginText);
	MY_UI::Safe_Delete(PasswordText);
	MY_UI::Safe_Delete(ConnectionStatus);
	MY_UI::Safe_Delete(ServerSelection);
	MY_UI::Safe_Delete(UI_LoginControl::self);
	FXAA.DeInit();
	Focus = Stopped;
}

void cLoginControl::Init(){
	OUTPUT_DEBUG_MSG("Init on login server");
	Focus=Running;
	FXAA.Init();
	ConnectButton = new MY_UI::Controls::Button(MY_UI::Internal::RootWidget);
	ConnectButton->SetPos(593, 388);
	ConnectButton->SetText( "Connect" );
	ConnectButton->SetPopup("Click here to start the demo connecting to the server");
	ConnectButton->OnLeftClicked.Add(std::bind(&cLoginControl::Connect, this) );

	LoginText = new MY_UI::Controls::TextBox(MY_UI::Internal::RootWidget);
	LoginText->Text->SetText( "Login" );
	LoginText->SetPos( 440, 388 );
	LoginText->SetLength(100);
	LoginText->SetPopup("Enter your login here. Max length of the login is 11 characters.");
	LoginText->SetMaxCharacters(LOGINLENGTH -1);
	LoginText->SetName("Login Box");

	PasswordText = new MY_UI::Controls::TextBox(MY_UI::Internal::RootWidget);
	PasswordText->Text->SetText( "Password" );
	PasswordText->SetPos(440, 450);
	PasswordText->SetLength(100);
	PasswordText->SetPopup("Enter your Password here. Max length of the password is 11 characters.");
	PasswordText->SetMaxCharacters(LOGINLENGTH -1);
	PasswordText->SetName("password Box");

	Sound_Engine::StopAll();
	//Background_music = Sound_Engine::LoadSound("alchemists_tale.xwma");
	//Background_music->Play(true);
	MY_UI::Safe_Delete(UI_LoginControl::self);
	UI_LoginControl::self = new UI_LoginControl(MY_UI::GetApplicationLayer(), this);
	
}
void cLoginControl::Connect( ){
	//ConnectTo("192.168.2.2", 4000);
	ConnectTo("127.0.0.1", 4000);
	if(ConnectionStatus) MY_UI::Safe_Delete(ConnectionStatus);
	ConnectionStatus = new MY_UI::Controls::Text(MY_UI::Internal::RootWidget);
	ConnectionStatus->SetColor(MY_UI::Utilities::White);
	ConnectionStatus->SetSize(500, 100);
	ConnectionStatus->SetPos(300, 600);
	ConnectionStatus->SetText("Establishing Connection to login server . . .");
}


void cLoginControl::Run(){
	if(Focus == Stopped) return;
	else if((Focus == ShuttingDown) | (Focus == ShutDownAll)){
		unsigned int temp = Focus;
		DeInit();
		if(temp == ShutDownAll){
			Focus = ShutDownAll;
		}
		return;
	}
	cBaseUDP_Client::Run();
	static unsigned int laststate = 4567;// invalid state
	if(ConnectionStatus) {
		if(Peer){
			if(Peer->State != laststate){
				ConnectionStatus->SetText(Network::Peer_State[Peer->State]);
				laststate=Peer->State;
				OUTPUT_DEBUG_MSG(Network::Peer_State[Peer->State]);
			}
		}
	}
}

void cLoginControl::LogintoServer(){
	std::string txt= ConnectButton->GetText();

	unsigned char selected =255;
	for (size_t i(0); i< GameServers.size(); i++){
		std::string gserver = GameServers[i].Name;
		if(gserver == txt){// found the server wanted
			selected = (unsigned char)i;
			break;
		}
	}
	if(selected == 255) OUTPUT_DEBUG_MSG("Ohh crap couldnt find the server, This is bad!!");
	cDataPacket pac(1);
	*pac.Data = selected;// log into the first server
	pac.SetID(Network::LS_LOGINTO_GAMESERVER_REQUEST);
	ReliableSendTo(pac);
}
void cLoginControl::SecureConnectionEstablished(cPeer* peer){
	OUTPUT_DEBUG_MSG("Secure Connection established, sending login information");
	ConnectionStatus->SetText("Secure Connection established, sending login information");

	cDataPacket packet(LOGINLENGTH*2);
	char* temp = packet.Data;
	packet.SetID(Network::LS_LOGIN_REQUEST);// header
	
	std::string login = LoginText->Text->GetText();
	login.resize(LOGINLENGTH-1);// force a valid length
	memcpy(temp, login.c_str() , login.size()+1);// copy the login + the null character
	login = PasswordText->Text->GetText();
	login.resize(LOGINLENGTH-1);// force a valid length
	memcpy(temp + LOGINLENGTH, login.c_str() , login.size()+1);// copy the login + the null character
	ReliableEncryptedSendTo(packet);
}
void cLoginControl::Disconnect(cPeer* peer){
	if(ConnectionStatus) {
		ConnectionStatus->SetText("Could not connect to the Login Server");
		OUTPUT_DEBUG_MSG("Could not connect to the Login Server");
	}
	MY_UI::Safe_Delete(ServerSelection);
	cBaseUDP_Client::Disconnect(peer);
}
void cLoginControl::ReceivePacket(cDataPacket* packet,cPeer* peer, size_t channel){
	switch(packet->GetID()){
		case(Network::LS_LOGIN_SUCCESS):
			OUTPUT_DEBUG_MSG("Login to Login Server Successful");
			ConnectionStatus->SetText("Login to Login Server Successful");
			break;
		case(Network::LS_ACCOUNT_LOGGED_IN):
			OUTPUT_DEBUG_MSG("Account Already Logged In");
			ConnectionStatus->SetText("Account Already Logged In");
			break;
		case(Network::LS_BAD_ACCOUNT_INFO):
			OUTPUT_DEBUG_MSG("Bad Account Info");
			ConnectionStatus->SetText("Bad Account Info");
			break;
		case(Network::LS_ACCOUNT_NOT_ACTIVE):
			OUTPUT_DEBUG_MSG("Account Not Active");
			ConnectionStatus->SetText("Account Not Active");
			break;
		case(Network::LS_SERVER_LISTING):
			fServer_Listing(packet);
			break;
		case(Network::LS_LOGINTO_GAMESERVER_SUCCESS):
			OUTPUT_DEBUG_MSG("Login to Game Server Successful");
			ConnectionStatus->SetText("Login to Game Server Successful");
			fLoginto_Gameserver_Success(packet);
			break;
		case(Network::LS_LOGINTO_GAMESERVER_FAILURE):
			OUTPUT_DEBUG_MSG("Unable to log into game server");
			ConnectionStatus->SetText("Unable to log into game server");
			break;
	};
}

void cLoginControl::fServer_Listing(cDataPacket* packet){
	char* data= packet->Data;
	uint16_t numberofservers = static_cast<uint16_t>(*data++);
	GameServers.resize(numberofservers);
	OUTPUT_DEBUG_MSG("Received Game Server Listing: /nNumber of Servers is: "<<numberofservers);
	
	MY_UI::Safe_Delete(ServerSelection);
	ServerSelection = new MY_UI::Controls::Window(MY_UI::Internal::RootWidget);
	ServerSelection->SetPos( 100, 100);
	ServerSelection->SetSize( 400, 300);
	ServerSelection->SetTitle("Server Selection");

	MY_UI::Controls::Text* label = new MY_UI::Controls::Text( ServerSelection );
	label->SetText("Server");
	label->SetSizeToContents();
	label->SetPos( 10 , 10);

	label = new MY_UI::Controls::Text( ServerSelection );
	label->SetText("Status");
	label->SetSizeToContents();
	label->SetPos( 140, 10 );

	label = new MY_UI::Controls::Text( ServerSelection );
	label->SetText("Number of Users");
	label->SetSizeToContents();
	label->SetPos( 200, 10);
	ServerSelection->TitleBar->Close_Button->OnLeftClicked.Add(std::bind(&cBaseUDP_Client::DisconnectPeer, this));

	for(size_t i(0); i<GameServers.size(); i++){

		memcpy(&GameServers[i].Name, data, LOGINLENGTH);
		data+=LOGINLENGTH;

		std::string servname = GameServers[i].Name;

		memcpy(&GameServers[i].Status, data, sizeof(uint16_t));
		data+=sizeof(uint16_t);

		std::string status;
		if(GameServers[i].Status == Network::ACTIVE) status="UP";
		else status="DOWN";
		
		memcpy(&GameServers[i].NumberOfUsers, data, sizeof(uint16_t));
		data+=sizeof(uint16_t);

		std::ostringstream ok;
		ok<<GameServers[i].NumberOfUsers;
		std::string nusers = ok.str();
		
		memcpy(&GameServers[i].IpAddress, data, sizeof(uint32_t));
		data+=sizeof(uint32_t);	

		MY_UI::Controls::Button* btn = new MY_UI::Controls::Button(ServerSelection);
		btn->SetText(servname);
		btn->SetPos(10, 10 + ((static_cast<int>(i) +1)*25));
		std::string tip = "Connect to the server, '" + servname + "'";
		btn->SetPopup(tip);
		btn->OnLeftClicked.Add( std::bind(&cLoginControl::LogintoServer, this) );

		MY_UI::Controls::Text* label = new MY_UI::Controls::Text( ServerSelection );
		label->SetText(status);
		label->SetSizeToContents();
		label->SetPos( 140, 10 + ((static_cast<int>(i) +1)*25));

		label = new MY_UI::Controls::Text( ServerSelection );
		label->SetText(nusers);
		label->SetSizeToContents();
		label->SetPos( 200, 10 + ((static_cast<int>(i) +1)*25));

	}

}
void cLoginControl::fLoginto_Gameserver_Success(cDataPacket* packet){// switch to 3d stuff
	size_t server = static_cast<size_t>( *(packet->Data));
	if( server < GameServers.size()){// dont want to access beyond the array
		if(GameServers[server].Status == Network::ACTIVE){
			OUTPUT_DEBUG_MSG("Login Into Game Server.. here we go!!");
			std::string login= LoginText->Text->GetText();
			login.resize(LOGINLENGTH-1);// force a valid length
			GameServerIP = GameServers[server].IpAddress;
			Login = login;
			Key.resize(Peer->EncryptionInfo.AESKey.SizeInBytes());
			memcpy(&Key[0], Peer->EncryptionInfo.AESKey.BytePtr(), Peer->EncryptionInfo.AESKey.SizeInBytes());
			Focus=ShuttingDown;
		}
	} else OUTPUT_DEBUG_MSG("could not log into the game server, even though the login server granted us access. Uhh uhh");
}

