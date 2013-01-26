#ifndef CCHATCHANNEL_H
#define CCHATCHANNEL_H
#include <set>
#include <map>
#include <vector>
#include <iostream>
#include "../Utilities/MyMath.h"
#include "../Utilities/NetLib/cDataPacket.h"
#include "../Common_Game_Objects_Lib/cUser.h"
#include <concurrent_unordered_map.h>


#define MAXCHATUSERSPERCHANNEL 254  //  it is not 255, because the moderator is not counted in here
class cChatChannel{
public:

	class cUserInfo{
	public:
		std::set<std::string*> Channels;
		std::vector<cDataPacket> Messages;
		uint16_t Id;
		std::string* Name;
		cUserInfo(): Id(65000), Name(0) {}
	};
	class cChannel{
	public:

		std::map<std::string*, uint16_t> Users;
		std::set<std::string> BannedUsers;// banned users cannot use a pointer because the ban needs to persist across a log in and log out
		std::string *Moderator, Password, Name;
		uint16_t ModeratorId;
		size_t NamesSize;
		std::vector<cDataPacket> Messages;
		cChannel(): NamesSize(0) {}
	};
	std::string DefaultModerator;
	/*
	Concurrency::concurrent_unordered_map<std::string, cChannel> Channels;
	Concurrency::concurrent_unordered_map<uint16_t, cUserInfo> IdToName;

	typedef Concurrency::concurrent_unordered_map<std::string, cChannel>::const_iterator const_Channel_accessor;
	typedef tbb::concurrent_hash_map<std::string, cChannel>::accessor Channel_accessor;
	typedef tbb::concurrent_hash_map<uint16_t, cUserInfo>::const_accessor const_IdToName_accessor;
	typedef tbb::concurrent_hash_map<uint16_t, cUserInfo>::accessor IdToName_accessor;

	typedef tbb::concurrent_hash_map<std::string, cChannel>::const_iterator const_Channel_iterator;
	typedef tbb::concurrent_hash_map<std::string, cChannel>::iterator Channel_iterator;
	typedef tbb::concurrent_hash_map<uint16_t, cUserInfo>::const_iterator const_IdToName_iterator;
	typedef tbb::concurrent_hash_map<uint16_t, cUserInfo>::iterator IdToName_iterator;

	cChatChannel();
	void Register(string* name, uint16_t peerid);
	void UnRegister(uint16_t peerid);

	void CreateChannel(cDataPacket* packet, cPeer* peer);

	void JoinChannel(cDataPacket* packet, cPeer* peer);
	void JoinChannel(std::string channel, std::string password, IdToName_accessor& userinfo);

	void LeaveChannel(cDataPacket* packet, cPeer* peer);
	void LeaveChannel(std::string channel, IdToName_accessor& userinfo);

	void AddTextToChannel(cDataPacket* packet, cPeer* peer);
	void SetModerator(Channel_accessor& channelinfo, std::string* name, uint16_t id);

	void FillChannelInfo(cChatChannel::cChannel& channel, IdToName_accessor& userinfo, uint16_t header);
	void LeftEnteredChannel(cDataPacket& packet, cChatChannel::cChannel& channel, IdToName_accessor& userinfo, uint16_t header);

	void ClearChatMessages();
	void ClearTells();
	*/

	/*
	uint16_t Tell(std::string& from, uint32_t& to, char* text, uint32_t& size);
	uint16_t Tell(std::string& from, std::string& to, char* text, uint32_t& size);
	//uint16_t Tell(uint32_t& id, char* text, uint32_t& size);
	template<class T>void Tell(cMessage& msg, T& iterator);// internal function

	uint16_t LeaveChannel(uint32_t& channel, uint32_t& id);
	uint16_t Kick(uint32_t& me, uint32_t& kickee, uint32_t& channel);
	uint16_t Ban(uint32_t& me, uint32_t& kickee, uint32_t& channel);

	void MassTell(char* text, uint32_t& size);
	*/


};

#endif
