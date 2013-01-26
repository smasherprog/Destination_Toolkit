
#include "cCursor.h"
#include "cItem.h"

//mutators

void cCursor::ConstructForVisual(char*& buffer){}
void cCursor::DeconstructForVisual(char*& buffer)const {}
size_t cCursor::DeconstructForVisualSize()const {return 0;}

void cCursor::Construct(char*& buffer){
	clear();
	uint32_t s(*reinterpret_cast<uint32_t*>(buffer));
	buffer+=sizeof(uint32_t);
	for(uint32_t i(0); i<s; i++){
		cItem* it = new cItem();
		it->Construct(buffer);
		Stack.insert(Stack.end(), it);
	}
}
void cCursor::clear(){
	for(std::deque<cItem*>::iterator beg(Stack.begin()); beg!= Stack.end(); beg++){
		delete *beg;
	}
	Stack.clear();
}
bool cCursor::push(cItem* item){
	Stack.insert(Stack.begin(), item);
	return true;
}
cItem* cCursor::pop(){
	if(!Stack.empty()){
		cItem* t(Stack.front());
		Stack.pop_front();
		return t;
	} else {
		return NULL;
	}
}
//accessors
void cCursor::Deconstruct(char*& buffer) const {
	*reinterpret_cast<size_t*>(buffer)=Stack.size();
	buffer+=sizeof(size_t);
	for(std::deque<cItem*>::const_iterator beg(Stack.begin()); beg!= Stack.end(); beg++){
		(*beg)->Deconstruct(buffer);
	}
}
size_t cCursor::DeconstructSize() const{
	size_t size(0);
	size+=sizeof(uint32_t);
	for(std::deque<cItem*>::const_iterator beg(Stack.begin()); beg!= Stack.end(); beg++){
		size+=(*beg)->DeconstructSize();
	}
	return size;
}
