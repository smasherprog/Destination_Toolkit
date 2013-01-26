#ifndef CWORLD_DEFINES_H
#define CWORLD_DEFINES_H

#define NO_GID 0xffffffff 
// this is a file for world-wide defines n stuff
#define MAXBAGSLOTS 8 // maximum slots a bag is allowed right now
#define DefaultAttackRate .25 // in seconds
#define DefaultAttackSpeed 2.0 
#define DefaultAttackRange 2.0

#define RADIANSPERSEC .75f
#define RESPAWNRATE 60*5 // in seconds
#define WALK_SPEED 3.0f
#define RUN_SPEED 40.0f
#define NAMESIZE 16 
#define MAXOBJECTEFFECTS 4 
#define MAXCHARACTEREFFECTS 4
#define MAXSTACKSIZE 5000
#define MAXCHARACTERS 10
#define INVALID_CHARACTER_INDEX 255

enum Movment_States { STOP_ROTATION=0xf0, ROTATE_LEFT=1, ROTATE_RIGHT=2, STOP_MOVEMENT = 0x0f, WALK_FORWARD = (1<<5), RUN_FORWARD = (1<<6), WALK_BACKWARD = (1<<7) };

enum Character_Types { NPC=0, PC };
enum Trade_Skills { BlackSmith, Fletching, NUMBEROFSKILLS };
enum Classes { Warrior, Cleric };
enum Races { Human, Dark_Elf };
enum Statistics { STRENGTH, STAMINCA, DEXTERITY, AGILITY, INTELLEGENCE, WISDOM, CHARISMA, 
	MagicResistance, FireResistance, ColdResistance, DiseaseResistance, PoisonResistance, 
	ARMORCLASS, HEALTH, MANA, ATTACK, NUMBEROFSTATS
};
enum BaseItemType { CONTAINER, OneHandedSlashing, TwoHandedeSlashing, OneHandedBlunt, TwoHandedBlunt, Bow, Wand, Shield, Throwing, STACKABLE,
	// non stackables go above this, stackable goes below
	Potion,  Ammo,Wood, Stone, Gem, Money, Metal, Armor, Food, Component, MaxBaseType 
};
enum Slot {LeftEar, Head, Face, RightEar, RightWeapon, NUMOFINVINTORYSLOTS};
struct cUVHelper{
	union {
		unsigned char tex[4];// tex0index, tex0alpha, tex1index, tex1alpha
		unsigned int t;
	};
};
struct cTimeStamp{
	unsigned int timestamp;
};

#define HEIGHTMAPFILE "Land"
#define	UVMAPFILE "UvMap"
#define	LANDSHADOWMAPFILE "ShadowMap"


#define TERRAIN_PATCH_SIZE 128.0f
#define TERRAIN_GRID_LEVELS 6
#define NUMROWS 64
#define NUMCOLS NUMROWS
#define PATCHSIZE 64
#define MAPFILESIZE 8192 // the number of array positions in one dimension of the world. The world is a square so the rows and columns are the same
#define MAPFILESIZEBITS MAPFILESIZE -1
#define MAX_VIEW_DISTANCE 4096
#define MAPBUFFER 1024
#define TERRAIN_TEX_SIZE 1024
#define PATCHINDICES 6*(TERRAIN_TEX_SIZE-1)*(TERRAIN_TEX_SIZE-1)
#define CELLSPACING 8 // the amount of cell spacing per array pos
#define WORLDSIZE (MAPFILESIZE*CELLSPACING) // the actual world size 
#define OCEAN_GRIDSIZE 64

inline void WrapCoord(float& coord) { if(coord >= WORLDSIZE) coord -= WORLDSIZE; else if(coord < 0.0f) coord += WORLDSIZE; }
inline void WrapCoord(float& coordx, float& coordy, float& coordz) { WrapCoord(coordx); WrapCoord(coordy); WrapCoord(coordz);  }

struct Packed_Uvs{// not really uvs.. but to do with them
	union {
		unsigned char Indices, Alpha;// indices has two index packed and alpha is the alpha level of the first texture index
		unsigned char buff[2];
	};
};

#endif