#include "cContainer.h"
#include "cItem.h"



void cContainer::ConstructForVisual(char*& buffer){}
void cContainer::DeconstructForVisual(char*& buffer)const {}
size_t cContainer::DeconstructForVisualSize()const{ return 0; }

void cContainer::Construct(char*& buffer){
	clear();
	Capacity=*buffer++;
	SlotsSize=*buffer++;
	ItemsSize=*buffer++;
	Items = new cItem*[Capacity];
	uint8_t end(ItemsSize);
	for(uint8_t i(0); i < end; i++) {
		cItem* it = new cItem();
		it->Construct(buffer);
		insert( it, i, Parent);
	}
}

void cContainer::Deconstruct(char*& buffer) const{
	*buffer++=Capacity;
	*buffer++=SlotsSize;
	*buffer++=ItemsSize;
	uint8_t end(Capacity);
	for(uint8_t i(0); i < end; i++) 
		if(Items[i]) Deconstruct(buffer);
}
size_t cContainer::DeconstructSize() const {
	size_t size(sizeof(uint8_t)*3);// cap, size, size
	uint8_t end(Capacity);
	for(uint8_t i(0); i < end; i++) 
		if(Items[i]) size +=DeconstructSize();
	return size;
}
void cContainer::Init(uint8_t size, cItem* parent){
	clear();
	Parent = parent;
	Capacity =size;
	Items = new cItem*[size];
	SlotsSize=size;
	memset(Items, 0, sizeof(cItem*)*size);
}

void cContainer::clear(){
	for(uint8_t i(0); i< Capacity; i++) delete Items[i];
	delete [] Items;
	Items=0;
	Capacity=0;
	SlotsSize=0;
	ItemsSize=0;
	Parent=0;
}
bool cContainer::insert(cItem* item, uint8_t slot, cItem* parent){
	if(slot >= Capacity) return false;
	cItem* temp(Items[slot]);
	if(!temp){// the slot is empty
		Items[slot] =item;
		ItemsSize++;
		if(item->Type == CONTAINER ) item->Contents.Parent=Parent;// notice different parents here
		uint32_t t(item->Size);
		parent->NotifyParentsOfInsertion(item, t);// notice different parents here
		return true;
	} else if(temp->Stackable() && temp->Size < MAXSTACKSIZE){
		temp->Size++;
		uint32_t t(item->Size);
		parent->NotifyParentsOfInsertion(item, t);// notice different parents here
		return true;
	}
	return false;
}
cItem* cContainer::remove(uint8_t slot, cItem* parent){
	if(slot >= Capacity) return false;
	cItem* t(Items[slot]);
	if(t){
		Items[slot]=NULL;// succesfull removal
		uint32_t s(t->Size);
		parent->NotifyParentsOfRemoval(t, s);// first notify of removal
		if(t->Type == CONTAINER) t->Contents.Parent=t;/// set this as the root bag
		ItemsSize--;
		return t;
	}
	return NULL;
}

