#ifndef CCHATWINDOW_H
#define CCHATWINDOW_H

#include "../MY_UI/Window.h"
#include <string>

class cDemoControl;
class cDataPacket;
class cPeer;
namespace MY_UI{
	namespace Controls{
		class List_Box;
		class Text_Box;
		class Text_Window;
	};
};
class cChatWindow : public MY_UI::Controls::Window{
public:

	static cChatWindow* self;
	cChatWindow(cWidgetBase* parent, cDemoControl* r);
	virtual ~cChatWindow();
	cDemoControl& DemoControl;

	void PlayerNotRegistered(cDataPacket* packet, cPeer* peer);
	void TooManyChannels(cDataPacket* packet, cPeer* peer);
	void ChannelAlreadyExists(cDataPacket* packet, cPeer* peer);
	void ChannelDoesNotExist(cDataPacket* packet, cPeer* peer);
	void AlreadyInChannel(cDataPacket* packet, cPeer* peer);
	void NotInChannel(cDataPacket* packet, cPeer* peer);
	void PasswordIncorrect(cDataPacket* packet, cPeer* peer);
	void BannedFromChannel(cDataPacket* packet, cPeer* peer);
	void SetModerator(cDataPacket* packet, cPeer* peer);
	void TooManyUsers(cDataPacket* packet, cPeer* peer);

	void JoinedChannel(cDataPacket* packet, cPeer* peer);
	void ChatMessage(cDataPacket* packet, cPeer* peer);
	void UserJoinedChannel(cDataPacket* packet, cPeer* peer);
	void UserLeftChannel(cDataPacket* packet, cPeer* peer);

	void AddChatMessageToWindow(std::string msg, std::string channel);
	void JoinCreateChannel(std::string channel, std::string password, unsigned short header);


	void Send_Chat_Message( );

	void JoinChatChannel( );
	void CreateChatChannel( );
	void UpdateMembersList( );

	class cChannel{
	public:
		std::set<std::string> Users;
		std::string Moderator, Password, Name;
	};
	std::vector<cChatWindow::cChannel> ChatWindows;

	MY_UI::Controls::List_Box* MembersList;
	MY_UI::Controls::TextBox* InputText;
	MY_UI::Controls::Text_Window* TextWindow;
};

#endif