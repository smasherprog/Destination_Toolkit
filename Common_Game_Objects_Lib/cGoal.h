#ifndef MISC_H
#define MISC_H
#include <list>
#include "cFunction.h"
#include "../Utilities/MyMath.h"

using namespace std;

class cGoal: public cFunction{
public:

	list<cFunction> FunctionStack;
	list<vec3> Path;
	cGoal();
	void push(uint32_t function);
	void push();
	void pop();
	void Construct(char*& buffer);
	void Construct(ifstream& stream);
	void Deconstruct(char*& buffer) const;
	void Deconstruct(ofstream& stream)const ;
	size_t DeconstructSize();
	void clear();
	
};
#endif
