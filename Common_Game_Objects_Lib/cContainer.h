#ifndef CCONTAINER_H
#define CCONTAINER_H
#include "../Utilities/MyMath.h"
#include <fstream>
#include <vector>
#include "cWorldObject.h"

class cItem;

class cContainer: public cSerialization{
	cContainer& operator=(const cContainer& rhs);
	cContainer(const cContainer& obj);
public:
	uint8_t Capacity, SlotsSize, ItemsSize;
	cItem **Items, *Parent;

	cContainer(): Capacity(0), SlotsSize(0), ItemsSize(0), Items(0), Parent(0) { }
	virtual ~cContainer(){ clear(); }

	void Init(uint8_t size, cItem* parent);
	void Construct(char*& buffer, cItem* parent){ Parent = parent; Construct(buffer); };

	virtual void clear();

	virtual void Construct(char*& buffer);
	virtual void ConstructForVisual(char*& buffer);

	virtual void Deconstruct(char*& buffer) const;
	virtual void DeconstructForVisual(char*& buffer)const ;

	virtual size_t DeconstructForVisualSize()const;
	virtual size_t DeconstructSize() const;

	bool insert(cItem* item, uint8_t slot, cItem* parent);
	cItem* remove(uint8_t slot, cItem* parent);

};




#endif

