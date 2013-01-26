#include "stdafx.h"
#include "Global_GameData.h"

std::shared_ptr<cCharacter> Characters[MAXCHARACTERS];
std::map<uint16_t, CharacterAndMesh> Players;
uint16_t PlayerID;
uint8_t ActiveCharacter;

std::shared_ptr<cCharacter> MyCharacter;//copy, do not delete
std::shared_ptr<cMesh> MyCharacterMesh;//copy, do not delete

vec3 SunDir(0.0f, .3f, 1.0f), SunPos;
float SunSpeed(.00005f), SunTime(.5f);
