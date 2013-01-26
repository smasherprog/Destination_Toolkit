#include "cInvintory.h"
#include "cItem.h"


void cInvintory::ConstructForVisual(char*& buffer){}
void cInvintory::DeconstructForVisual(char*& buffer)const{}
size_t cInvintory::DeconstructForVisualSize()const{ return 0;}

void cInvintory::clear(){
	for(uint32_t i(0); i< NUMOFINVINTORYSLOTS; i++) delete Invintory[i];	
	memset(Invintory, 0, sizeof(Invintory)); 
	Size =0;
}
void cInvintory::Construct(char *&buffer){
	clear();
	uint8_t slot = *buffer++;
	uint8_t mask(0x80);
	for(uint8_t i(0); i < NUMOFINVINTORYSLOTS; i++){
		if(mask&slot){
			Invintory[i] = new cItem();
			Invintory[i]->Construct(buffer);
			++Size;
		}
		mask>>=1;
	}
}
//accessors
void cInvintory::Deconstruct(char *&buffer) const{
	uint8_t mask(0x80), *slots(reinterpret_cast<uint8_t*>(buffer));
	buffer++;// skip this 
	*slots=0;// zero it out
	for(uint8_t i(0); i < NUMOFINVINTORYSLOTS; i++){
		if(Invintory[i]){
			*slots|=mask;// mark the item
			Invintory[i]->Deconstruct(buffer);
		}
		mask>>=1;
	}
}

size_t cInvintory::DeconstructSize() const{
	size_t size=1;// number of bytes needed for the mask
	for(uint8_t i(0); i<NUMOFINVINTORYSLOTS; i++){
		if(Invintory[i]){
			size+=Invintory[i]->DeconstructSize();
		}
	}
	return size;
}

bool cInvintory::insert(cItem *item, const uint8_t slot){
	if(NUMOFINVINTORYSLOTS >= slot){ 
		return false;// buffer overruun
	} else{
		Invintory[slot]=item; // good insert
		Size++;
		return true;
	} 
}
cItem* cInvintory::remove(const uint8_t slot){
	if(NUMOFINVINTORYSLOTS>= slot){
		return NULL;
	} else{
		--Size;
		cItem* it = Invintory[slot]; // good insert
		Invintory[slot] = NULL;
		return it;
	} 
}