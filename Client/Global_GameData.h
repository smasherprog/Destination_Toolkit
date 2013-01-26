#ifndef GLOBAL_GAMEDATA_H
#define GLOBAL_GAMEDATA_H
#include "../Common_Game_Objects_Lib/cWorldDefines.h"
#include "../Common_Game_Objects_Lib/cCharacter.h"
#include <map>
#include <memory>
#include "../Graphics_Lib/cMesh.h"

class CharacterAndMesh{
public:
	CharacterAndMesh() :Character(0), CharacterMesh(0) {}
	std::shared_ptr<cCharacter> Character;
	std::shared_ptr<cMesh> CharacterMesh;
};
enum ControlStates {  Stopped, Running, ShuttingDown, ShutDownAll };

extern std::shared_ptr<cCharacter> Characters[MAXCHARACTERS];
extern std::map<uint16_t, CharacterAndMesh> Players;
extern uint16_t PlayerID;
extern uint8_t ActiveCharacter;

extern std::shared_ptr<cCharacter> MyCharacter;//copy, do not delete
extern std::shared_ptr<cMesh> MyCharacterMesh;//copy, do not delete

extern vec3 SunDir, SunPos;
extern float SunSpeed, SunTime;


#endif