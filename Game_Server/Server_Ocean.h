#ifndef SERVER_OCEAN_H
#define SERVER_OCEAN_H
#include "../Common_Game_Objects_Lib/Base_Ocean.h"

class cGameServer;

class Server_Ocean: public Base_Ocean{
public:

	Server_Ocean(cGameServer* game);
	virtual ~Server_Ocean() { }
	cGameServer& GameServer;

	virtual void GridAdded(const vec3& tlpos);
	virtual void Add_Ocean_Grid(char* data, unsigned int size, size_t channel);

};


#endif