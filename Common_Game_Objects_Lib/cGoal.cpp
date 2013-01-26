#include "stdafx.h"
#include "cGoal.h"
#include "cFunction.h"

cGoal::cGoal(){}
void cGoal::push(uint32_t function){
	Function=function;
	push();
}
void cGoal::push(){
	FunctionStack.insert(FunctionStack.begin(), static_cast<cFunction>(*this));// *this should be sliced down.. if not I will make a copy contstructor
}
void cGoal::pop(){
	if(!FunctionStack.empty()){// not empty
		cFunction::operator=(static_cast<cFunction>(*this));
		FunctionStack.pop_front();
	}
}
void cGoal::Construct(char*& buffer){
	size_t s(*reinterpret_cast<size_t*>(buffer));
	buffer+=sizeof(size_t);
	for(size_t i(0); i<s; i++){
		vec3 t;
		t=*reinterpret_cast<vec3*>(buffer);
		buffer+=sizeof(vec3);
		Path.insert(Path.end(), t);
	}
	size_t q(*reinterpret_cast<size_t*>(buffer));
	buffer+=sizeof(size_t);
	for(size_t i(0); i< q; i++){
		FunctionStack.insert(FunctionStack.begin(), cFunction(buffer));
	}
}
void cGoal::Construct(ifstream& stream){
	size_t s;
	stream>>s;
	for(size_t i(0); i<s; i++){
		vec3 t;
		stream>>t.x>>t.y>>t.z;
		Path.insert(Path.end(), t);
	}
	size_t q;
	stream>>q;
	for(size_t i(0); i< q; i++){
		FunctionStack.insert(FunctionStack.begin(), cFunction(stream));
	}
}
void cGoal::Deconstruct(char*& buffer) const{
	*reinterpret_cast<size_t*>(buffer)=Path.size();
	buffer+=sizeof(size_t);
	for(list<vec3>::const_iterator beg(Path.begin()); beg!= Path.end(); beg++){
		*reinterpret_cast<vec3*>(buffer)=*beg;
		buffer+=sizeof(vec3);
	}
	*reinterpret_cast<size_t*>(buffer)=FunctionStack.size();
	buffer+=sizeof(size_t);
	for(list<cFunction>::const_iterator beg(FunctionStack.begin()); beg!= FunctionStack.end(); beg++){
		beg->Deconstruct(buffer);
	}
}
void cGoal::Deconstruct(ofstream& stream) const{
	stream<<Path.size();
	for(list<vec3>::const_iterator beg(Path.begin()); beg!= Path.end(); beg++){
		stream<<beg->x<<beg->y<<beg->z;
	}
	stream<<FunctionStack.size();
	for(list<cFunction>::const_iterator beg(FunctionStack.begin()); beg!= FunctionStack.end(); beg++){
		beg->Deconstruct(stream);
	}
}
size_t cGoal::DeconstructSize(){
	size_t size(sizeof(size_t)*2);// size of path and functionstack size's
	size+=( ((sizeof(float)*3)*Path.size()) + (sizeof(cFunction)*FunctionStack.size()));
	return size;
}
void cGoal::clear(){
	Path.clear();
	FunctionStack.clear();
}