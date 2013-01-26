#include "stdafx.h"
#include "Server_Ocean.h"
#include "../Utilities/NetLib/cDataPacket.h"
#include "../Common_Game_Objects_Lib/cBaseNetwork.h"
#include "cGameServer.h"

Server_Ocean::Server_Ocean(cGameServer* game): GameServer(*game) {}

void Server_Ocean::GridAdded(const vec3& tlpos){
	vec2 t(tlpos.x, tlpos.z);
	cDataPacket* pack = new cDataPacket((char*)&t, sizeof(vec2));
	pack->SetID(Network::GS_CONF_OCEAN_GRID);
	GameServer.AddChange(pack);
}

void Server_Ocean::Add_Ocean_Grid(char* data, unsigned int length, size_t channel) {
	OUTPUT_DEBUG_MSG("trying to add ocean grid to server");
	if(length != sizeof(vec2)) return;// ignore bad packets  should disconnect the peer here

	float x = *reinterpret_cast<float*>(data);
	data+=sizeof(float);
	float y = *reinterpret_cast<float*>(data);
	vec3 tlpos(x, 0, y);
	WrapCoord(tlpos.x, tlpos.y,tlpos.z);
	if(AddGrid(tlpos)){
		OUTPUT_DEBUG_MSG("successfully added ocean grid to server to pos "<<tlpos);
		GridAdded(tlpos);
	}
}