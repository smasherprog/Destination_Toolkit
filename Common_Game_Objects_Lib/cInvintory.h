#ifndef CINVINTORY_H
#define CINVINTORY_H
#include <fstream>
#include "cWorldDefines.h"
#include "../Utilities/MyMath.h"
#include "cWorldObject.h"
class cItem;

class cInvintory : public cSerialization{
	cInvintory(const cInvintory& rhs);
	cInvintory& operator=(const cInvintory& rhs);
public:

	uint8_t Size;
	cItem* Invintory[NUMOFINVINTORYSLOTS];
	
	cInvintory(): Size(0) { memset(Invintory, 0, sizeof(Invintory)); }
	virtual ~cInvintory(){ clear();} 

	virtual void clear();

	virtual void Construct(char*& buffer);
	virtual void ConstructForVisual(char*& buffer);

	virtual void Deconstruct(char*& buffer) const;
	virtual void DeconstructForVisual(char*& buffer)const;

	virtual size_t DeconstructForVisualSize()const;
	virtual size_t DeconstructSize() const;


	cItem* operator[](const uint8_t slot){ if(slot >= NUMOFINVINTORYSLOTS) return NULL; else return Invintory[slot]; }

	bool insert(cItem* item, const uint8_t slot);
	cItem* remove(const uint8_t slot);
	
};

#endif
