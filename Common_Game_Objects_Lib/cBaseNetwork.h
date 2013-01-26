#ifndef CBASENETWORK
#define CBASENETWORK
#include <set>

class cInfo;
class cDataPacket;
namespace Network{
	
	#define LOGINLENGTH 12// max login size
	enum AccountTypes{ USER_ACCOUNT= '0', GM_ACCOUNT = '1', SERVER_ACCOUNT = '2', ADMIN_ACCOUNT = '3', DUMMY_ACCOUNT = '4' };
	enum AccountStatus{ ACTIVE = '1', NOTACTIVE = '0'};

	#define INVALID_SERVER_INDEX 255// there cannot be this many servers, so its a good invalid index
	#define MSGBUFFER 255 // so I can send the size as a uint8 
	#define MAX_JOINED_CHANNELS 16
	#define MAX_CHANNEL_LENGTH 16

	class cServerInfo{
	public:
		cServerInfo(): Status(NOTACTIVE), NumberOfUsers(0) {}
		char Name[LOGINLENGTH];
		unsigned short int Status, NumberOfUsers;
		unsigned int IpAddress;
	};
	
	class cServerInfo_S : public cServerInfo{
	public:
		unsigned short int PeerIndex;
		std::set<cInfo*> Users;
	};
	enum LoginServerPacketTypes { 
		LS_LOGIN_REQUEST, LS_LOGIN_SUCCESS, LS_ACCOUNT_LOGGED_IN, LS_BAD_ACCOUNT_INFO, LS_ACCOUNT_NOT_ACTIVE, LS_CREATE_SERVER, LS_SERVER_LISTING,
		LS_LOGINTO_GAMESERVER_REQUEST, LS_LOGINTO_GAMESERVER_SUCCESS, LS_LOGINTO_GAMESERVER_FAILURE, LS_USER_LOGINTO_GAMESERVER, LS_USER_LOGGEDOUT_GAMESERVER, /*

		LS_Player_Not_LoggedOn, LS_Player_Banned, LS_Player_Kicked, LS_Password_Changed, LS_Bad_Password, LS_Account_Created, LS_Cannot_Create_Account, LS_Account_Allready_Exisits, 
		LS_Account_Updated, LS_Account_DNE,  LS_Server_Status, LS_UnRecognized_Command, LS_Server_Status_Updated, LS_LogOut_Of_Success, LS_LogOut_Of_Falure, 
		LS_Remove_User, LS_Log_Out_Of, LS_Log_Out, LS_Log_In_To, LS_Kick_, LS_Ban_, LS_Change_Password, LS_Create_Account, 
		LS_Update_Account,  LS_Update_Server_Status, */  LS_MaxFunctions 
	};
	enum GameServerPacketTypes { // there are some overlap between the login server and this, but the overlap cannot exist together
		GS_LOGIN_REQUEST, GS_LOGIN_SUCCESS, GS_CHARACTER_LISTING, GS_INVALID_CHARACTER, GS_ALREADY_IN_WORLD, GS_CREATE_CHARACTER, GS_ENTER_WORLD, GS_LOGINTO_WORLD,
		GS_WALK_FORWARD, GS_STOP_MOVEMENT, GS_RUN_FORWARD, GS_WALK_BACKWARD, GS_ROTATE_LEFT, GS_ROTATE_RIGHT, GS_STOP_ROTATION, GS_POSITION_UPDATE,
		CHAT_PLAYER_NOT_REGISTERED, CHAT_TOO_MANY_CHANNELS, CHAT_CHANNEL_ALREADY_EXISTS, CHAT_CREATE_CHANNEL, CHAT_CHANNEL_CREATED, CHAT_CHANNEL_DOESNOT_EXIST, CHAT_JOINED_CHANNEL,
		CHAT_ALREADY_IN_CHANNEL, CHAT_NOT_IN_CHANNEL, CHAT_PASSWORD_INCORRECT, CHAT_JOIN_CHANNEL, CHAT_CHANNEL_MESSAGE, CHAT_TOO_MANY_USERS,
		CHAT_BANNED_FROM_CHANNEL, CHAT_USER_JOINED_CHANNEL, CHAT_USER_LEFT_CHANNEL, CHAT_SET_MODERATOR, CHAT_ADD_TEXT_TO_CHANNEL,
		GS_WORLD_STATE, GS_WORLD_UPDATE, GS_CHARACTER_CHANGES , GS_CHARACTER_ENTERED_WORLD, GS_CHARACTER_LEFT_WORLD, 



		GS_REQ_OCEAN_GRID, GS_CONF_OCEAN_GRID,
		/*GS_LoginInTo_Suceess, GS_LoginInTo_Failure, GS_Not_Enough_Permission,
		GS_Player_Not_LoggedOn, GS_Player_Banned, GS_Player_Kicked, GS_Password_Changed, GS_Bad_Password, GS_Account_Created, GS_Cannot_Create_Account, GS_Account_Allready_Exisits, 
		GS_Account_Updated, GS_Account_DNE,  GS_Server_Status, GS_UnRecognized_Command, GS_Server_Status_Updated, GS_LogOut_Of_Success, GS_LogOut_Of_Falure, 
		GS_Remove_User, GS_Log_Out_Of, GS_Log_Out, GS_Log_In_To, GS_Kick_, GS_Ban_, GS_Change_Password, GS_Create_Account, 
		GS_Update_Account,  GS_Update_Server_Status,  */ GS_MaxFunctions 
	};

};


#endif