#ifndef CCURSOR_H
#define CCURSOR_H
#include "cWorldObject.h"
#include <deque>
class cItem;

class cCursor : public cSerialization{
	cCursor(const cCursor& rhs);
	cCursor& operator=(const cCursor& rhs);
public:
	
	std::deque<cItem*> Stack;
	
	cCursor() {}
	virtual ~cCursor(){ clear(); }
	
	virtual void clear();

	virtual void Construct(char*& buffer);
	virtual void ConstructForVisual(char*& buffer);

	virtual void Deconstruct(char*& buffer) const;
	virtual void DeconstructForVisual(char*& buffer)const ;

	virtual size_t DeconstructForVisualSize()const;
	virtual size_t DeconstructSize() const;

	bool push(cItem* item);
	cItem* pop();
};


#endif
