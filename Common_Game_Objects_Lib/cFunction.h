#ifndef CFUNCTION_H
#define CFUNCTION_H

#include "../Utilities/MyMath.h"
//#include "../BV_Lib/cAABB.h"
#include "../Utilities/My_Timer.h"
#include <fstream>

using namespace std;
class cItem;

class cFunction{
public:

	uint32_t Function;
	My_Timer::tick_count Time;
	float Radius;
	bool Preserve;
	double Duration;
	cItem* Item;
	vec3 Position;
	//cAABB BV;
	//cCharacter* Character;
	//cObject* Object;
	//cStructure* Structure;

	cFunction();
	cFunction(const cFunction& func);
	cFunction& operator=(const cFunction& rhs);
	cFunction(ifstream& stream);
	cFunction(char*& buffer);
	void Construct(char*& buffer);
	void Construct(ifstream& stream);
	void Deconstruct(char*& buffer) const;
	void Deconstruct(ofstream& stream) const ;
	static uint32_t DeconstructSize();

};

#endif