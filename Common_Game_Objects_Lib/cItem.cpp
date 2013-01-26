#include "cItem.h"

//Size(0), Position2(0), Sid(NO_GID) { memset(Slot, 0, sizeof(Slot) );  }

void cItem::Construct(char*& buffer){
	clear();
	cWorldObject::Construct(buffer);
	Size =  *reinterpret_cast<uint32_t*>(buffer);
	buffer+=sizeof(uint32_t);
	Position2 =  *reinterpret_cast<uint16_t*>(buffer);
	buffer+=sizeof(uint16_t);
	Sid =  *reinterpret_cast<uint32_t*>(buffer);
	buffer+=sizeof(uint32_t);
	uint8_t slot = *buffer++;
	uint8_t mask(0x80);
	for(uint8_t i(0); i < MAXBAGSLOTS; i++){
		if(mask&slot){
			Slot[i]= new cWorldObject();
			Slot[i]->Construct(buffer);
		}
		mask>>=1;
	}
	if(Type == CONTAINER) Contents.Construct(buffer, this);
}
void cItem::Deconstruct(char*& buffer) const {
	cWorldObject::Deconstruct(buffer);
	
	*reinterpret_cast<uint32_t*>(buffer)=Size;  
	buffer+=sizeof(uint32_t);
	*reinterpret_cast<uint16_t*>(buffer)=Position2;
	buffer+=sizeof(uint16_t);
	*reinterpret_cast<uint32_t*>(buffer)=Sid;
	buffer+=sizeof(uint32_t);

	uint8_t mask(0x80), *slots(reinterpret_cast<uint8_t*>(buffer));
	buffer++;// skip this 
	*slots=0;// zero it out
	for(uint8_t i(0); i < MAXBAGSLOTS; i++){
		if(Slot[i]){
			*slots|=mask;// mark the item
			Slot[i]->Deconstruct(buffer);
		}
		mask>>=1;
	}
	if(Type == CONTAINER) Contents.Deconstruct(buffer);
}


void cItem::ConstructForVisual(char*& buffer) {}

void cItem::DeconstructForVisual(char*& buffer) const {}

size_t cItem::DeconstructForVisualSize() const { return 0; }

size_t cItem::DeconstructSize() const {
	size_t size( cWorldObject::DeconstructSize()  + sizeof(Size) + sizeof(Position2) + sizeof(Sid) );
	for(uint8_t i(0); i < MAXBAGSLOTS; i++) {
		if(Slot[i]) size+= Slot[i]->DeconstructSize();
	}
	if(Type == CONTAINER) size += Contents.DeconstructSize();
	return size;
}
void cItem::clear(){
	cWorldObject::clear();
	Contents.clear();
	for(uint8_t i(0); i < MAXBAGSLOTS; i++) {
		delete Slot[i];
	}
	memset(Slot, 0, sizeof(Slot));
	Position2=0;
	Sid=NO_GID;
	Size=0;
}

bool cItem::insert(cItem* item){
	if(Type!=CONTAINER) return false;// trying to insert something into a non container type.. wtf
	uint8_t i(0);
	do{// first check for the first open slot or an opporunity to stack, if that fails we must search throughthe bags.. Ugh
		if(!Contents.Items[i]) return Contents.insert(item, i, this);
		else if( item->CanStackOnTopOf(Contents.Items[i])) return Contents.insert(item, i, this);
	} while(++i!= Contents.Capacity);
	i=0;
	cItem* temp(0);
	do{// the bag is full so we can just go through all the items checking for a bag
		temp = Contents.Items[i];
		if(temp)
			if(temp->Type == CONTAINER) temp->insert(item);
	} while(++i!= Contents.Capacity);
	return false;// if we search through all the bags and cant insert anything return false
}
bool cItem::insert(cItem *item, uint8_t *path, const uint8_t depth){ 
	cItem* temp(this);
	uint8_t i(0);
	do{
		if(temp->Type == CONTAINER && path[i] < temp->Contents.Capacity) {
			if(i == depth){// we are done searching, try to insert here
				return temp->Contents.insert(item, path[i], temp);
			} // otherwise we havet hit yet
		} else return false;
	} while(++ i < depth);
	return false;
}
void cItem::NotifyParentsOfInsertion(cItem* item, uint32_t& size){// this ia  bottom up notification
	cItem* temp(this);
	for( ; ; ){
		temp->Weight+= item->Weight;
		temp->Size += item->Size;
		cItem* t(temp->Contents.Parent);
		if(t == temp) break;// we have hit the top of the parent
		temp = t;
	}
}
void cItem::NotifyParentsOfRemoval(cItem* item, uint32_t& size){// this ia  bottom up notification this can lead to errors here. I need a better way!!!
	cItem* temp(this);
	for( ; ; ){
		temp->Weight-= item->Weight;
		temp->Size -= item->Size;
		cItem* t(temp->Contents.Parent);
		if(t == temp) break;// we have hit the top of the parent
		temp = t;
	}
}
cItem* cItem::remove(uint8_t *path, const uint8_t depth){  
	cItem* temp(this);
	uint8_t i(0);
	do{
		if(temp->Type == CONTAINER && path[i] < temp->Contents.Capacity) {
			temp = temp->Contents.Items[path[i]];
			if(!temp) return NULL;
			else if(i == depth){// we are done searching, try to insert here
				return temp->Contents.remove(path[i], temp);
			} // otherwise we havet hit yet
		} else return false;
	} while(++ i < depth);
	return NULL;
}
cItem* cItem::GetItem(uint8_t *path, const uint8_t depth) {
	cItem* temp(this);
	uint8_t i(0);
	do{
		if(temp->Type == CONTAINER && path[i] < temp->Contents.Capacity) {
			temp = temp->Contents.Items[path[i]];
			if(!temp) return NULL;
			else if(i == depth) return temp;// we are done searching, try to return here
		} else return false;
	} while(++ i < depth);
	return NULL;
}
