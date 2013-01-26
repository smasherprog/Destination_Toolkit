#ifndef CITEM_H
#define CITEM_H

#include <fstream>
#include "cWorldDefines.h"
#include "cWorldObject.h"
#include "cContainer.h"
#include "../Utilities/MyMath.h"
#include <mutex>


class cItem: public cWorldObject{
	cItem(const cItem& obj);
	cItem& operator=(cItem& rhs);
public:

	cContainer Contents;
	uint32_t Sid;
	
	uint16_t Position2;// this is a BAG position as two uint8's
	cWorldObject* Slot[MAXBAGSLOTS];

	uint32_t Size;
	std::mutex Mutex;

	cItem() : Size(0), Position2(0), Sid(NO_GID) { memset(Slot, 0, sizeof(cWorldObject*)*MAXBAGSLOTS);  }
	virtual ~cItem() { clear(); }

	virtual void clear();

	virtual void Construct(char*& buffer);
	virtual void ConstructForVisual(char*& buffer);

	virtual void Deconstruct(char*& buffer) const;
	virtual void DeconstructForVisual(char*& buffer)const;

	virtual size_t DeconstructForVisualSize()const;
	virtual size_t DeconstructSize() const;


	bool insert(cItem* item);
	bool insert(cItem *item, uint8_t *path, const uint8_t depth);

	cItem* remove(uint8_t *path, const uint8_t depth);
	cItem* GetItem(uint8_t *path, const uint8_t depth);

	bool Stackable() const { return Type >= STACKABLE; }
	bool CanStackOnTopOf(const cItem* item) const { return (this->Type == item->Type) & item->Stackable(); }

	friend cContainer;
protected:

	void NotifyParentsOfInsertion(cItem* item, uint32_t& size);
	void NotifyParentsOfRemoval(cItem* item, uint32_t& size);

};

#endif
