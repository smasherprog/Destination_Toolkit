#ifndef CHATE_H
#define CHATE_H
#include "../Utilities/My_Timer.h"

template<class T>class cHate {
public:
	T* Target;
	uint32_t Hate, DamageDone;
	My_Timer::tick_count LastHateTime;

cHate(){
	clear();
}
void clear(){
	Target=&DummyCharacter;
	Hate=DamageDone=0;
	LastHateTime=cMyTime::g_Time;
}
cHate& operator=(const cHate& rhs){
	Target=rhs.Target;
	Hate=rhs.Hate;
	DamageDone=rhs.DamageDone;
	LastHateTime=rhs.LastHateTime;
	return *this;
}
};

#endif