#ifndef CCHARACTER_H
#define CCHARACTER_H
#include "../BV_Lib/cAABB.h"
#include "cGoal.h"
#include "cInvintory.h"
#include "cCursor.h"
#include "cHate.h"
#include <map>
#include <fstream>
#include "cWorldObject.h"
#include "cItem.h"
#include "../Utilities/MyMath.h"
#include <mutex>

struct CharacterChange
	//: public cTimeStamp
	{
	vec3 pos;
	float look;
	uint16_t gid;
	uint8_t movementstate;
};

class cCharacter: public cWorldObject{
	cCharacter& operator=(cCharacter& rhs);
	cCharacter(const cCharacter& obj);
public:
	
	/*
	static std::string Directory, FilePostFix, ClassName;


	uint16_t InfoBits;
	uint8_t Class_Race, Level;
	uint32_t Experience;

	uint8_t MovementState;
	float Look;

	cInvintory Invintory;
	cItem BackPack, Bank;
	cCursor Cursor;

	std::mutex Mutex;

	cCharacter(){ clear(); }
	virtual ~cCharacter() {}

	void Run();
	//the following functions set the movement state and return if the state changed.
	bool StopMovement()	{ Speed=0.0f;			uint8_t temp = MovementState; MovementState&=STOP_MOVEMENT; return temp != MovementState; }
	bool WalkFoward()	{ Speed=WALK_SPEED;		uint8_t temp = MovementState; MovementState&=STOP_MOVEMENT; MovementState|=WALK_FORWARD; return temp != MovementState; }
	bool WalkBackward()	{ Speed=WALK_SPEED;		uint8_t temp = MovementState; MovementState&=STOP_MOVEMENT; MovementState|=WALK_BACKWARD; return temp != MovementState; }
	bool RunFoward()	{ Speed= RUN_SPEED;		uint8_t temp = MovementState; MovementState&=STOP_MOVEMENT; MovementState|=RUN_FORWARD; return temp != MovementState; }
	bool StopRotation()	{ uint8_t temp = MovementState; MovementState&=STOP_ROTATION; return temp != MovementState; }
	bool RotateLeft()	{ uint8_t temp = MovementState; MovementState&=STOP_ROTATION; MovementState|=ROTATE_LEFT; return temp != MovementState; }
	bool RotateRight()	{ uint8_t temp = MovementState; MovementState&=STOP_ROTATION; MovementState|=ROTATE_RIGHT; return temp != MovementState; }
	uint8_t GetRotationState() { return MovementState&STOP_MOVEMENT; }
	uint8_t GetForwardState() { return MovementState&STOP_ROTATION; }
	//the following functions actually move the character using the states set above
	
	bool Created(){ return (InfoBits&0x8000)!=0; }
	void SetCreated(){ InfoBits|=0x8000; }

	bool InsertInInvintory(cItem* item, uint8_t slot);
	bool InsertInBackPack(cItem* item, uint8_t* path, uint8_t depth);
	bool InsertInBackPack(cItem* item);// expensive function...
	bool InsertOnCursor(cItem* item);

	cItem* RemoveFromInvintory(uint8_t slot);
	cItem* RemoveFromBackPack(uint8_t* path, uint8_t depth);
	cItem* RemoveFromCursor();

	cItem* GetInInvintory(uint8_t slot);
	uint16_t CarryCapacity();

	virtual void clear();

	virtual void Construct(char*& buffer);
	virtual void ConstructForVisual(char*& buffer);

	virtual void Deconstruct(char*& buffer) const;
	virtual void DeconstructForVisual(char*& buffer)const ;

	virtual size_t DeconstructForVisualSize()const;
	virtual size_t DeconstructSize() const;

protected:
	// internal functions
	void Rotate_Left();
	void Rotate_Right();
	void Walk_Forward();
	void Run_Forward();
	void Walk_Backward();
	*/
};

#endif
