#include "cCharacter.h"
#include "../Utilities/My_Timer.h"

/*
std::string cCharacter::Directory="cCharacter", cCharacter::FilePostFix=".cha", cCharacter::ClassName="cCharacter";

void cCharacter::Run(){
	uint8_t walk = MovementState&STOP_ROTATION;
	uint8_t rot = MovementState&STOP_MOVEMENT;
	switch(rot){
		case(ROTATE_LEFT):
			Rotate_Left();
			break;
		case(ROTATE_RIGHT):
			Rotate_Right();
			break;
	};
	switch(walk){
		case(WALK_FORWARD):
			Walk_Forward();
			break;
		case(RUN_FORWARD):
			Run_Forward();
			break;
		case(WALK_BACKWARD):
			Walk_Backward();
			break;
	};
}
void cCharacter::Walk_Forward(){
	vec3 look(0.0f, 0.0f, 1.0f);
	mat4 r;
	r.setupRotateY(Look);
	look*=r;

	Position +=  (look* (My_Timer::DT*WALK_SPEED));
}
void cCharacter::Run_Forward(){
	vec3 look(0.0f, 0.0f, 1.0f);
	mat4 r;
	r.setupRotateY(Look);
	look*=r;
	Position +=  (look* (My_Timer::DT*RUN_SPEED));
}
void cCharacter::Walk_Backward(){
	vec3 look(0.0f, 0.0f, 1.0f);
	mat4 r;
	r.setupRotateY(Look);
	look*=r;
	Position -=  (look* (My_Timer::DT*WALK_SPEED));
}

void cCharacter::Rotate_Left(){
	Look-=RADIANSPERSEC*My_Timer::DT*Pi;
}
void cCharacter::Rotate_Right(){
	Look+=RADIANSPERSEC*My_Timer::DT*Pi;
}

bool cCharacter::InsertInInvintory(cItem *item, const uint8_t slot){
	if(Invintory.insert(item, slot)){
		Weight+=item->Weight;
		for(uint32_t i(0); i< NUMBEROFSTATS; i++){
			Stats[i]+=item->Stats[i];
		}
		Stats[HEALTH]+=item->Stats[HEALTH];
		Stats[MANA]+=item->Stats[MANA];
		return true;
	} else {
		return false;
	}
}
bool cCharacter::InsertInBackPack(cItem *item, uint8_t *path, const uint8_t depth){
	if(BackPack.insert(item, path, depth)){
		Weight+=item->Weight;
		return true;
	} else {
		return false;
	}
}
cItem* cCharacter::RemoveFromInvintory(uint8_t slot){
	cItem* item(Invintory.remove(slot));
	if(item){
		Weight-=item->Weight;
		for(uint32_t i(0); i< NUMBEROFSTATS; i++){
			Stats[i]-=item->Stats[i];
		}
		Stats[HEALTH]-=item->Stats[HEALTH];
		Stats[MANA]-=item->Stats[MANA];
	}
	return item;
}
cItem* cCharacter::RemoveFromBackPack(uint8_t *path, const uint8_t depth){
	cItem* item(BackPack.remove(path, depth));
	if(item){
		Weight-=item->Weight;
	}
	return item;
}
bool cCharacter::InsertOnCursor(cItem *item){
	Cursor.push(item);
	Weight+=item->Weight;
	return true;
}
cItem* cCharacter::RemoveFromCursor(){
	cItem* item(Cursor.pop());
	if(item){
		Weight-=item->Weight;
	}
	return item;
}
cItem* cCharacter::GetInInvintory(uint8_t slot){
	return Invintory[slot];
}
bool cCharacter::InsertInBackPack(cItem *item){
	if(BackPack.insert(item)){// expensive function..... NPC USE ONLY!!
		Weight+=item->Weight;
		return true;
	} else {
		return false;
	}
}
uint16_t cCharacter::CarryCapacity(){
	return (Stats[STRENGTH]>>1) + Stats[STRENGTH];
}

void cCharacter::clear(){
	cWorldObject::clear();
	InfoBits=0;
	MovementState=0;
	Look=Pi/2.0f;//look down the positive z axis
	Class_Race=0;
	Experience=5;
	Level=1;

	BackPack.clear();
	Bank.clear();
	Cursor.clear();
	Invintory.clear();
}

size_t cCharacter::DeconstructSize() const { 
	return cWorldObject::DeconstructSize() + BackPack.DeconstructSize() + Bank.DeconstructSize() + Cursor.DeconstructSize() + Invintory.DeconstructSize()
	+ sizeof(InfoBits) +  sizeof(Class_Race) + sizeof(Level) + sizeof(Experience) + sizeof(Look); 
}
void cCharacter::Deconstruct(char*& buffer) const {
	cWorldObject::Deconstruct(buffer);
	BackPack.Deconstruct(buffer);
	Cursor.Deconstruct(buffer);
	*reinterpret_cast<uint16_t*>(buffer) = InfoBits;
	buffer+=sizeof(InfoBits);

	*reinterpret_cast<uint8_t*>(buffer) = Class_Race;
	buffer+=sizeof(Class_Race);
	*reinterpret_cast<uint8_t*>(buffer) = Level;
	buffer+=sizeof(Level);
	*reinterpret_cast<uint32_t*>(buffer) = Experience;
	buffer+=sizeof(Experience);
	*reinterpret_cast<float*>(buffer) = Look;
	buffer+=sizeof(Look);
}
void cCharacter::Construct(char*& buffer){
	cWorldObject::Construct(buffer);
	BackPack.Construct(buffer);
	Cursor.Construct(buffer);
	InfoBits=*reinterpret_cast<uint16_t*>(buffer);
	buffer+=sizeof(InfoBits);
	Class_Race=*reinterpret_cast<uint8_t*>(buffer);
	buffer+=sizeof(Class_Race);
	Level=*reinterpret_cast<uint8_t*>(buffer);
	buffer+=sizeof(Level);
	Experience=*reinterpret_cast<uint32_t*>(buffer);
	buffer+=sizeof(Experience);
	Look=*reinterpret_cast<float*>(buffer);
	buffer+=sizeof(Look);
}
size_t cCharacter::DeconstructForVisualSize() const {
	size_t size =cWorldObject::DeconstructForVisualSize();
	size += sizeof(InfoBits) + sizeof(Class_Race) + sizeof(Level) + sizeof(Look);
	return size;
}
void cCharacter::DeconstructForVisual(char*& buffer) const {// for sending the character to another to see in game, not all info is sent
	cWorldObject::DeconstructForVisual(buffer);
	*reinterpret_cast<uint16_t*>(buffer) = InfoBits;
	buffer+=sizeof(InfoBits);
	*reinterpret_cast<uint8_t*>(buffer) = Class_Race;
	buffer+=sizeof(Class_Race);
	*reinterpret_cast<uint8_t*>(buffer) = Level;
	buffer+=sizeof(Level);
	*reinterpret_cast<float*>(buffer) = Look;
	buffer+=sizeof(Look);
}
void cCharacter::ConstructForVisual(char*& buffer){// for sending the character to another to see in game 
	clear();// make sure the onbject is clean first...
	cWorldObject::ConstructForVisual(buffer);
	InfoBits=*reinterpret_cast<uint16_t*>(buffer);
	buffer+=sizeof(InfoBits);
	Class_Race=*reinterpret_cast<uint8_t*>(buffer);
	buffer+=sizeof(Class_Race);
	Level=*reinterpret_cast<uint8_t*>(buffer);
	buffer+=sizeof(Level);
	Look=*reinterpret_cast<float*>(buffer);
	buffer+=sizeof(Look);
}

*/