#include "stdafx.h"
#include "cChatChannel.h"
#include "cGameServer.h"
/*
cChatChannel::cChatChannel(){
	Channel_accessor ret;	
	std::string channel = "Default";
	Channels.insert(ret, channel);
	DefaultModerator ="";
	ret->second.Name = channel;// channel name
	ret->second.Password="";// no password
	ret->second.Moderator= &DefaultModerator;
	ret->second.ModeratorId =0;
}
void cChatChannel::Register(string* name, uint16_t peerid){
	cUserInfo u;
	u.Name = name;
	u.Id=peerid;
	IdToName.insert(tbb::concurrent_hash_map<uint16_t, cUserInfo>::value_type(peerid, u)); 
}
void cChatChannel::UnRegister(uint16_t peerid){
	IdToName_accessor userinfo;
	if(!IdToName.find(userinfo, peerid)) return;
	for(set<std::string*>::iterator beg(userinfo->second.Channels.begin()); beg!= userinfo->second.Channels.end(); beg++){// for all the channels that the user was in
		LeaveChannel(**beg, userinfo);
	}
	userinfo.release();
	IdToName.erase(peerid);
}

void cChatChannel::CreateChannel(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG("Chat::CreateChannel");
	IdToName_accessor userinfo;
	if(!IdToName.find(userinfo, peer->Id)) return;
	if(userinfo->second.Channels.size() >= MAX_JOINED_CHANNELS){// this should be thread safe because the only thread calling this should also be calling from the same account. Users cannot join channels on others' accounts
		uint16_t header = Network::CHAT_TOO_MANY_CHANNELS;
		return userinfo->second.Messages.push_back( cDataPacket((char*)&header, sizeof(uint16_t) ) );
	}
	if(!Network::VerifyPacketIntegrity(packet, 2)) return GameServer.DisconnectPeer(peer);// peer sent a bad packet, disconnect
	std::string channel, password;
	char* data = packet->Data + HEADERSIZE;
	uint8_t s = *data++;//number of bytes to read for the channel name
	if(s > MAX_CHANNEL_LENGTH) return GameServer.DisconnectPeer(peer);// peer sent a bad packet, disconnect
	channel.assign(data, s); 
	data+=s;// skip the channel name
	s = *data++;
	if(s > MAX_CHANNEL_LENGTH) return GameServer.DisconnectPeer(peer);// peer sent a bad packet, disconnect
	password.assign(data, s); 
	
	Channel_accessor chan;	
	
	if(!Channels.insert(chan, channel)) {// channel already exists...
		uint16_t header = Network::CHAT_CHANNEL_ALREADY_EXISTS;
		return userinfo->second.Messages.push_back( cDataPacket((char*)&header, sizeof(uint16_t) ) );
	} 
	chan->second.Name = channel;// channel name
	chan->second.Password=password;
	chan->second.Moderator= userinfo->second.Name;
	chan->second.ModeratorId = userinfo->second.Id;
	chan->second.NamesSize +=userinfo->second.Name->size();

	pair<std::set<std::string*>::iterator, bool> suc = userinfo->second.Channels.insert(&chan->second.Name);
	if(!suc.second){
		uint16_t header = Network::CHAT_ALREADY_IN_CHANNEL;
		return userinfo->second.Messages.push_back( cDataPacket((char*)&header, sizeof(uint16_t) ) );
	}
	FillChannelInfo(chan->second, userinfo, Network::CHAT_CHANNEL_CREATED);
	OUTPUT_DEBUG_MSG("Chat::Succesfully created channel");
}

void cChatChannel::JoinChannel(cDataPacket* packet, cPeer* peer){
	OUTPUT_DEBUG_MSG("Chat::JoinChannel00");
	IdToName_accessor userinfo;
	if(!IdToName.find(userinfo, peer->Id)) return;
	if(userinfo->second.Channels.size() >= MAX_JOINED_CHANNELS){// this should be thread safe because the only thread calling this should also be calling from the same account. Users cannot join channels on others' accounts
		uint16_t header = Network::CHAT_TOO_MANY_CHANNELS;
		return userinfo->second.Messages.push_back( cDataPacket((char*)&header, sizeof(uint16_t) ) );
	}
	if(!Network::VerifyPacketIntegrity(packet, 2)) return GameServer.DisconnectPeer(peer);// peer sent a bad packet, disconnect
	std::string channel, password;
	char* data = packet->Data + HEADERSIZE;
	uint8_t s = *data++;//number of bytes to read for the channel name
	if(s > MAX_CHANNEL_LENGTH) return GameServer.DisconnectPeer(peer);// peer sent a bad packet, disconnect
	channel.assign(data, s); 
	data+=s;// skip the channel name
	s = *data++;
	if(s > MAX_CHANNEL_LENGTH) return GameServer.DisconnectPeer(peer);// peer sent a bad packet, disconnect
	password.assign(data, s); 

	JoinChannel(channel, password, userinfo);
}
void cChatChannel::JoinChannel(std::string channel, std::string password, IdToName_accessor& userinfo){
	OUTPUT_DEBUG_MSG("Chat::JoinChannel01");
	Channel_accessor channelinfo;	

	if(!Channels.find(channelinfo, channel)) {
		uint16_t header = Network::CHAT_CHANNEL_DOESNOT_EXIST;
		return userinfo->second.Messages.push_back( cDataPacket((char*)&header, sizeof(uint16_t) ) );
	} 
	if(channelinfo->second.Users.size() > MAXCHATUSERSPERCHANNEL){
		uint16_t header = Network::CHAT_TOO_MANY_USERS;
		return userinfo->second.Messages.push_back( cDataPacket((char*)&header, sizeof(uint16_t) ) );
	} 
	if(channelinfo->second.Moderator == userinfo->second.Name) {	
		uint16_t header = Network::CHAT_ALREADY_IN_CHANNEL;
		return userinfo->second.Messages.push_back( cDataPacket((char*)&header, sizeof(uint16_t) ) );
	} 
	std::set<std::string>::iterator banned = channelinfo->second.BannedUsers.find(*(userinfo->second.Name));	
	if(banned != channelinfo->second.BannedUsers.end()){// user is banned!!
		uint16_t header = Network::CHAT_BANNED_FROM_CHANNEL;
		return userinfo->second.Messages.push_back( cDataPacket((char*)&header, sizeof(uint16_t) ) );
	} else { // the user not banned, and not in the channel
		std::pair<std::map<std::string*, uint16_t>::iterator, bool> na = channelinfo->second.Users.insert(std::map<std::string*, uint16_t>::value_type(userinfo->second.Name, userinfo->second.Id));
		if(!na.second){
			uint16_t header = Network::CHAT_ALREADY_IN_CHANNEL;
			return userinfo->second.Messages.push_back( cDataPacket((char*)&header, sizeof(uint16_t) ) );
		} else {// check password first
			if(password != channelinfo->second.Password){
				uint16_t header = Network::CHAT_PASSWORD_INCORRECT;
				return userinfo->second.Messages.push_back( cDataPacket((char*)&header, sizeof(uint16_t) ) );
			}// else insert the user and continue
		}	
		userinfo->second.Channels.insert(&channelinfo->second.Name);// insert the channel
		channelinfo->second.NamesSize+=userinfo->second.Name->size();
		channelinfo->second.Messages.push_back(cDataPacket());
		LeftEnteredChannel(channelinfo->second.Messages.back(), channelinfo->second, userinfo, Network::CHAT_USER_JOINED_CHANNEL);
		FillChannelInfo(channelinfo->second, userinfo, Network::CHAT_JOINED_CHANNEL);
	}
}
void cChatChannel::LeaveChannel(cDataPacket* packet, cPeer* peer){
	IdToName_accessor userinfo;
	if(!IdToName.find(userinfo, peer->Id)) return;
	if(!Network::VerifyPacketIntegrity(packet, 1)) return GameServer.DisconnectPeer(peer);// peer sent a bad packet, disconnect

	std::string channel;
	char* data = packet->Data + HEADERSIZE;
	uint8_t s = *data++;//number of bytes to read for the channel name
	if(s > MAX_CHANNEL_LENGTH) return GameServer.DisconnectPeer(peer);// peer sent a bad packet, disconnect
	channel.assign(data, s); 
	LeaveChannel(channel, userinfo);
}
void cChatChannel::LeaveChannel(std::string channel, IdToName_accessor& userinfo){
	Channel_accessor channelinfo;	

	if(!Channels.find(channelinfo, channel)) {
		uint16_t header = Network::CHAT_CHANNEL_DOESNOT_EXIST;
		return userinfo->second.Messages.push_back( cDataPacket((char*)&header, sizeof(uint16_t) ) );
	} 
	// erase the channel from the users list
	size_t na = channelinfo->second.Users.erase(userinfo->second.Name);
	if(na) return;// if the user is not in the channel get out
	// let the user know he left the channel
	userinfo->second.Messages.push_back(cDataPacket());
	LeftEnteredChannel(userinfo->second.Messages.back(), channelinfo->second, userinfo, Network::CHAT_USER_LEFT_CHANNEL);
	if(channelinfo->second.Users.size() ==0){// last user, destroy channel
		Channels.erase(channelinfo);
		return;
	}	
	channelinfo->second.NamesSize -=userinfo->second.Name->size();
	channelinfo->second.Messages.push_back(cDataPacket());
	// let everyone in the channel know the user left
	LeftEnteredChannel(channelinfo->second.Messages.back(), channelinfo->second, userinfo, Network::CHAT_USER_LEFT_CHANNEL);
	// if the user who left is the moderator make sure to elect a new moderator
	if(channelinfo->second.Moderator == userinfo->second.Name) {	
		std::string* st = channelinfo->second.Users.begin()->first;
		SetModerator(channelinfo, st, channelinfo->second.Users.begin()->second);
	} 
}

void cChatChannel::SetModerator(Channel_accessor& channelinfo, std::string* name, uint16_t id){
	channelinfo->second.Moderator = name;
	channelinfo->second.ModeratorId = id;
	channelinfo->second.Messages.push_back( cDataPacket(HEADERSIZE + static_cast<unsigned int>(name->size()) +1) );
	char* da = channelinfo->second.Messages.back().Data;
	*reinterpret_cast<uint16_t*>(da) = Network::CHAT_SET_MODERATOR;
	da+=sizeof(uint16_t);
	*da++ = static_cast<char>(name->size());
	memcpy(da, name->c_str(), name->size());
}

void cChatChannel::LeftEnteredChannel(cDataPacket& packet, cChatChannel::cChannel& channel, IdToName_accessor& userinfo, uint16_t header){
	packet.resize(HEADERSIZE + static_cast<unsigned int>(userinfo->second.Name->size()) + static_cast<unsigned int>(channel.Name.size())+ 2) ;
	// let everyone in the channel know that the user joined the channel
	char* da = packet.Data;
	*reinterpret_cast<uint16_t*>(da) =header;
	da+=sizeof(uint16_t);
	*da++ = static_cast<char>(channel.Name.size());
	memcpy(da, channel.Name.c_str(), channel.Name.size());
	da+=channel.Name.size();
	*da++ = static_cast<char>(userinfo->second.Name->size());
	memcpy(da, userinfo->second.Name->c_str(), userinfo->second.Name->size());
}
void cChatChannel::FillChannelInfo(cChatChannel::cChannel& channel, IdToName_accessor& userinfo, uint16_t header){
	//namesize is the number of bytes for all users, except the moderator, 
	userinfo->second.Messages.push_back( cDataPacket(static_cast<unsigned int>(channel.NamesSize + channel.Users.size() + channel.Name.size() +2 + channel.Moderator->size() +1) + HEADERSIZE) );
	char* p = userinfo->second.Messages.back().Data;
	*reinterpret_cast<uint16_t*>(p) =header;
	p+=HEADERSIZE;

	*p++ = static_cast<char>(channel.Name.size());
	memcpy(p, channel.Name.c_str(), channel.Name.size());// channel name first
	p+=channel.Name.size();

	*p++=(char)channel.Users.size();// max users per channel is 254... so this is fine
	// always write the moderator first, this way, everyone knows, the first name is the moderator
	*p++ = static_cast<char>(channel.Moderator->size());

	memcpy(p, channel.Moderator->c_str(), channel.Moderator->size());
	p+= channel.Moderator->size();
	for(std::map<std::string*, uint16_t>::iterator beg(channel.Users.begin()); beg!= channel.Users.end(); beg++){
		std::string* st = beg->first;
		*p++ = static_cast<char>(st->size());
		memcpy(p, st->c_str(), st->size());
		p+=st->size();
	}

}
void cChatChannel::AddTextToChannel(cDataPacket* packet, cPeer* peer){

	IdToName_accessor userinfo;
	if(!IdToName.find(userinfo, peer->Id)) return;

	if(!Network::VerifyPacketIntegrity(packet, 2)) return GameServer.DisconnectPeer(peer);// peer sent a bad packet, disconnect

	std::string channel;
	char* data = packet->Data + HEADERSIZE;
	uint8_t s = *data++;//number of bytes to read for the channel name
	if(s > MAX_CHANNEL_LENGTH) return GameServer.DisconnectPeer(peer);// user messing with us, get out
	channel.assign(data, s); 
	data+=s;// skip the channel name
	Channel_accessor channelinfo;	
	if(!Channels.find(channelinfo, channel)) return GameServer.DisconnectPeer(peer);// user messing with us, get out

	// see if the user is in the channel
	std::map<std::string*, uint16_t>::iterator founduser = channelinfo->second.Users.find(userinfo->second.Name);
	// check if the user is even in the channel
	if(founduser == channelinfo->second.Users.end() && userinfo->second.Name != channelinfo->second.Moderator) return GameServer.DisconnectPeer(peer);// user messing with us, get out
	else {// the user is in the channel, add the message
		s = *data++;//number of bytes to read for the message
		if(s >= MSGBUFFER) return GameServer.DisconnectPeer(peer);// user messing with us, get out and disconnect the peer
		std::string ms;
		ms.assign(data, s);
		OUTPUT_DEBUG_MSG(" In channel "<<channelinfo->second.Name<<" Adding the message "<<ms);
		channelinfo->second.Messages.push_back( cDataPacket(HEADERSIZE + 3 + static_cast<unsigned int>(channelinfo->second.Name.size() + userinfo->second.Name->size()) + s));
		char* p = channelinfo->second.Messages.back().Data;
		*reinterpret_cast<uint16_t*>(p) = Network::CHAT_CHANNEL_MESSAGE;
		p+=HEADERSIZE;
		// channel name
		*p++ = static_cast<char>(channelinfo->second.Name.size());
		memcpy(p, channelinfo->second.Name.c_str(), channelinfo->second.Name.size());
		p+=channelinfo->second.Name.size();
		//user name who generated the message
		*p++ = static_cast<char>(userinfo->second.Name->size());

		memcpy(p, userinfo->second.Name->c_str(), userinfo->second.Name->size());
		p+=userinfo->second.Name->size();
		//the message itself
		*p++ = s;
		memcpy(p, data, s);
	}
}
void cChatChannel::ClearChatMessages(){
	for(Channel_iterator beg(Channels.begin()); beg!=Channels.end(); beg++){
		beg->second.Messages.clear();
	}
}
*/