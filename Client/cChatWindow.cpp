#include "stdafx.h"
#include "cChatWindow.h"
#include "../Utilities\NetLib\cPeer.h"
#include "../Utilities\NetLib\cDataPacket.h"
#include "../MY_UI\TextBox.h"
#include "../MY_UI\List_Box.h"
#include "../Common_Game_Objects_Lib/cBaseNetwork.h"
#include "cDemoControl.h"

cChatWindow* cChatWindow::self=0;
cChatWindow::~cChatWindow(){
	self=0;
}
cChatWindow::cChatWindow(cWidgetBase* parent, cDemoControl* r) :Window(parent), DemoControl(*r) {
	assert(self ==0);
	SetTitle("Chat");
	SetSize(400, 300);
	SetPos(100, 500);

	TextWindow = new MY_UI::Controls::Text_Window(this);
	TextWindow->SetSize(300, 250);

	InputText = new MY_UI::Controls::TextBox( this );
	InputText->SetText( "" );
	InputText->SetLength(300);
	InputText->Align_Within_Parent(BOTTOM);
	InputText->SetMaxCharacters(250);
	InputText->OnReturnPressed.Add(std::bind(&cChatWindow::Send_Chat_Message, this));

	MembersList = new MY_UI::Controls::List_Box( this );
	MembersList->SetPos(10, 0);
	MembersList->SetSize(100, 295);
	MembersList->Align_Within_Parent(RIGHT);

	
	/*
	DockControlLeft = new Gwen::Controls::TabControl( this );
	DockControlLeft->Dock( Gwen::Pos::Fill);
	DemoControl=0;

	Gwen::Controls::TabButton* pButton = DockControlLeft->AddPage( L"Options" );
	Base* pPage = pButton->GetPage();
	
	Gwen::Controls::Label* label = new Gwen::Controls::Label( pPage );
	label->SetText( "Chat Layout" );
	label->SizeToContents();
	label->SetPos(10, 10);

	Gwen::Controls::RadioButtonController* pRadio = new Gwen::Controls::RadioButtonController( pPage );
	pRadio->SetBounds( 10, 30, 100, 100 );
	pRadio->AddOption( "Top" )->Select();
	pRadio->AddOption( "Bottom" );
	pRadio->AddOption( "Left" );		
	pRadio->AddOption( "Right" );
	pRadio->onSelectionChange.Add( this, &ThisClass::OnDockChange );
		
	label = new Gwen::Controls::Label( pPage );
	label->SetText( "Join Chat Channel" );
	label->SizeToContents();
	label->SetPos(115, 10);

	Gwen::Controls::TextBox* inpt = new Gwen::Controls::TextBox( pPage );

	inpt->SetText( "" );
	inpt->SetPos(115, 30);
	inpt->onTextChanged.Add(this, &ThisClass::JoinChannelChange);
	inpt->onReturnPressed.Add(this, &ThisClass::JoinChatChannel);

	label = new Gwen::Controls::Label( pPage );
	label->SetText( "Create Chat Channel" );
	label->SizeToContents();
	label->SetPos(115, 50);

	inpt = new Gwen::Controls::TextBox( pPage );
	inpt->SetKeyboardInputEnabled(true);
	inpt->SetText( "" );
	inpt->SetPos(115, 70);

	inpt->onReturnPressed.Add(this, &ThisClass::CreateChatChannel);
*/		
}

/*
void cChatWindow::JoinChatChannel( MY_UI::Controls::cWidgetBase* pControl ){
	Gwen::Controls::TextBox* inpt = (Gwen::Controls::TextBox*)pControl;
	if(inpt->TextLength() == 0 ) return;
	std::wstring txt(inpt->GetText());
	inpt->SetText("");
	std::string channel(txt.begin(), txt.end());
	JoinCreateChannel(channel, "", Network::CHAT_JOIN_CHANNEL);
}
void cChatWindow::CreateChatChannel( MY_UI::Controls::cWidgetBase* pControl ){
	Gwen::Controls::TextBox* inpt = (Gwen::Controls::TextBox*)pControl;
	if(inpt->TextLength() == 0 ) return;
	std::wstring txt(inpt->GetText());
	inpt->SetText("");
	std::string channel(txt.begin(), txt.end());
	JoinCreateChannel(channel, "", Network::CHAT_CREATE_CHANNEL);
}
*/
void cChatWindow::JoinedChannel(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG(" Received JoinedChannel packet");
	char* data = packet->Data;
	cChatWindow::cChannel chan;

	uint8_t size = *data++;
	chan.Name.assign(data, size);
	data+=size;

	size_t numofusers = (size_t)(*data++);

	size = *data++;// moderator namesize
	chan.Moderator.assign(data, size);
	data+=size;

	OUTPUT_DEBUG_MSG("chan.Moderator "<<chan.Moderator<<" mod size "<<(unsigned int)size<<" Num of users "<<numofusers<<" offset so far is "<<data - packet->Data);
	for(size_t i(0); i< numofusers; i++){
		size = *data++;
		std::string temp(data, size);
		OUTPUT_DEBUG_MSG("User: "<<temp<<" with a size of "<<(unsigned int)size);
		data+=size;
		chan.Users.insert(temp);
	}
	chan.Password = "";
	/*
	std::string channel(chan.Name.begin(), chan.Name.end());
	Gwen::Controls::TabButton* pButton = DockControlLeft->AddPage( wchannel);
	pButton->onPress.Add(this, &ThisClass::UpdateMembersList);
	Base* pPage = pButton->GetPage();
	Gwen::Controls::ListBox* TextOutput = new Gwen::Controls::ListBox( pPage );
	TextOutput->SetName(chan.Name);	
	chan.Window = TextOutput;
	ChatWindows.push_back(chan);
	TextOutput->Dock( Gwen::Pos::Fill );
	pButton->OnPress();// bring the tab to the front
	
	*/
	std::string msg =" You have entered the chat room, " + chan.Name;
	AddChatMessageToWindow(msg, chan.Name);
}
void cChatWindow::UpdateMembersList(  ){
	/*
	std::wstring wchannel = but->GetText();
	std::string channel(wchannel.begin(), wchannel.end());
	
	for(size_t i(0); i< ChatWindows.size(); i++){
		//if(ChatWindows[i].Name == channel){// found it
			
			//MembersList->Clear();// remove all currently listed members
			string temp = "(M)" + ChatWindows[i].Moderator;
			MembersList->AddItem(temp);
			OUTPUT_DEBUG_MSG("Clearing the members list and adding "<<temp<<" as a moderator");
			for(std::set<std::string>::iterator beg(ChatWindows[i].Users.begin()); beg != ChatWindows[i].Users.end(); beg++){
				temp = *beg;
				OUTPUT_DEBUG_MSG("adding user "<<temp<<" to the member list");
				MembersList->AddItem(temp);
			}		
			MembersList->ScrollToTop();
			return;
		//}
	}
	OUTPUT_DEBUG_MSG("didnt find channel "<<channel );
	*/
}
void cChatWindow::ChatMessage(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG(" Received ChatMessage packet");
	char* data = packet->Data;
	std::string channel, username, msg;
	uint8_t size = *data++;
	channel.assign(data, size);
	data+=size;
	size = *data++;
	username.assign(data, size);
	data+=size;
	size = *data++;
	msg.assign(data, size);
	std::string msg1=username + ":" + msg;
	AddChatMessageToWindow(msg1, channel);
}
void cChatWindow::JoinCreateChannel(std::string channel, std::string password, unsigned short header){
	if(channel.size() > MAX_CHANNEL_LENGTH){
		std::string temp;
		for(size_t i(0); i<MAX_CHANNEL_LENGTH; i++) temp.push_back(channel[i]);
		OUTPUT_DEBUG_MSG("Error in JoinChannel, the channel name "<<channel<<" is too long. Shortening the channel name to "<<temp);
		channel=temp;
	}
	if(password.size() > MAX_CHANNEL_LENGTH){
		std::string temp;
		for(size_t i(0); i<MAX_CHANNEL_LENGTH; i++) temp.push_back(channel[i]);
		OUTPUT_DEBUG_MSG("Error in JoinChannel, the password is too long. Shortening the password name to "<<MAX_CHANNEL_LENGTH<<" characters long");
		password=temp;
	}
	OUTPUT_DEBUG_MSG("Sending request to join channel "<<channel);
	cDataPacket pack((unsigned int)(channel.size() + password.size()) +2);
	char* data = pack.Data;
	pack.SetID(header);
	*data++=static_cast<char>(channel.size()); 
	memcpy(data, channel.c_str(), channel.size());
	data+=channel.size(); 
	*data++=static_cast<char>(password.size()); 
	memcpy(data, password.c_str(), password.size());
	DemoControl.ReliableSendTo(pack);
}

void cChatWindow::AddChatMessageToWindow(std::string msg, std::string channel){
	/*
	for(size_t i(0); i< ChatWindows.size(); i++){
		if(ChatWindows[i].Window->GetName() == channel){
			ChatWindows[i].Window->AddItem(msg);
			ChatWindows[i].Window->ScrollToBottom();
			break;
		}
	}
	*/
	TextWindow->Insert("C: " + channel + "\\" + msg); 
	OUTPUT_DEBUG_MSG(" AddChatMessageToWindow( "<<msg<<","<<channel<<")");
}
void cChatWindow::UserJoinedChannel(cDataPacket* packet, cPeer* peer){
	char* data = packet->Data;
	std::string channel, name;
	uint8_t size=*data++;
	channel.assign(data, size);
	data+=size;
	size=*data++;
	name.assign(data, size);
	AddChatMessageToWindow("Player " + name + " has joined the channel. ", channel);	
}
void cChatWindow::UserLeftChannel(cDataPacket* packet, cPeer* peer){
	char* data = packet->Data;
	std::string channel, name;
	uint8_t size=*data++;
	channel.assign(data, size);
	data+=size;
	size=*data++;
	name.assign(data, size);
	AddChatMessageToWindow("Player " + name + " has left the channel. ",channel);
}
void cChatWindow::Send_Chat_Message( ){
	std::string txt1 =InputText->GetText();
	//Gwen::Controls::TabButton* pButton = DockControlLeft->GetCurrentButton();
	

	std::string channel = "Default";
	if(channel == "Layout") return;
	InputText->SetText("");// clear the text window
	OUTPUT_DEBUG_MSG("Sending the chat message: "<<txt1<<" to channel "<<channel);
	cDataPacket pack((unsigned int)(txt1.size() + 2 + channel.size()));
	char* beg = pack.Data;
	pack.SetID(Network::CHAT_ADD_TEXT_TO_CHANNEL);

	// attach the channel name
	*beg++ = static_cast<char>(channel.size());// channelname size
	memcpy(beg, channel.c_str(), channel.size());	
	beg+=channel.size();
	//now send the message
	*beg++ = static_cast<char>(txt1.size());// channel text size
	memcpy(beg, txt1.c_str(), txt1.size());
	DemoControl.ReliableSendTo(pack);
}
void cChatWindow::PlayerNotRegistered(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG("Player not registered");
}
void cChatWindow::TooManyChannels(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG("Too many channels");
}
void cChatWindow::ChannelAlreadyExists(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG("Channel Already Exists");
}
void cChatWindow::ChannelDoesNotExist(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG("Channel Does Not Exist");
}
void cChatWindow::AlreadyInChannel(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG("Already In Channel");
}
void cChatWindow::NotInChannel(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG("Channel Already Exists");
}
void cChatWindow::PasswordIncorrect(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG("Password Incorrect");
}
void cChatWindow::BannedFromChannel(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG("Banned From Channel");
}
void cChatWindow::SetModerator(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG("Set Moderator");
}
void cChatWindow::TooManyUsers(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG("Too Many Users");
}
