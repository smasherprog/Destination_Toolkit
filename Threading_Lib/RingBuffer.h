#ifndef CRINGBUFFER_H
#define CRINGBUFFER_H

/*
 *
 * Ring Buffer is part of the Threading Library
 * Copyright (c) 2011 NoLimitsDesigns
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 4. This License is good for the current version of the Threading Library which 
 *	  is currently 1.01. This does not preclude newer versions from 
 *    holding a differnet license agreement; however, previous versions 
 *    shall maintain their original license agreement.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.

 * If there are any concerns or questions about the code, please e-mail smasherprog@gmail.com or visit www.nolimitsdesigns.com
 *

 *
 * Author: Scott Lee
 */
#include <atomic>

#define RingBufferSize 256// this is the number of work slots allocated for each thread. It is constant for the entire programs execution, so dont go too low!!

template<class T> class AtomicRingBuffer{
public:
	AtomicRingBuffer(): head_(0), tail_(0), Size(0), SRWLock(0) { }
	bool push(T value){	// this is a push_front
		while(SRWLock.exchange(true, memory_order_acquire)) {}// wait for unlock
		unsigned int head = head_;
		unsigned int next_head=next(head_);
		if (next_head==tail_ ){
			SRWLock.store(false, memory_order_release);
			return false;
		}
		head_=next_head;// advance the head
		++Size;
		SRWLock.store(false, memory_order_release);
		ring_[head]=value;
		return true;
	}
	bool pop(T& value){// this is a 
		while(SRWLock.exchange(true, memory_order_acquire)) {}// wait for unlock
		unsigned int tail = tail_;
		if (tail_== head_ ) {
			SRWLock.store(false, memory_order_release);
			return false;
		}
		tail_=next(tail_);
		--Size;
		SRWLock.store(false, memory_order_release);
		value=ring_[tail];
		return true;
	}
	unsigned int Size;

private:
	unsigned int next(unsigned int current) {return  (current >= RingBufferSize -1 ? 0 : current+1);}
	T ring_[RingBufferSize];
	unsigned int head_, tail_;
	std::atomic<bool> SRWLock;
};

/*!
this is a deque replacement that I wrote trying to get some speedup over the std version. Most of the std functions are here
 still working on it, so it is not 100% accurate yet
 THiS USES FREE ON PURPOSE since i use it within my allocation library, I had to get away from new and delete to avoid errors
*/
template<class T> class RingBuffer{
public:
	
	void push_front(T& value){	//!< this inserts at the head
		T* next_head=previous(head_);
		if (next_head==tail_ ){// resize
			resize(Capacity*2);// grow by two
			next_head=previous(head_);
		} 
			
		head_=next_head;// advance the head
		*head_ = value;// then write the element
		++Size;		
	}
	void push_back(T& value){//!< this inserts at the tail
		T* tail = tail_;
		T* prev_tail=next(tail);
		if (next_head==tail_ ){// resize
			resize(Capacity*2);// grow to twice the size
			tail = tail_;
			prev_tail=next(tail);
		}
		*tail_ = value;	
		tail_=prev_tail;// advance the head
		++Size;
	}
	T& front(){ return *head_; }
	T& back(){ return *previous(tail_); }//!< the tail is always one passed the end
	void pop_front(){
		if (tail_== head_ ) return;// empty			
		head_=next(head_);// move the head 
		--Size;// decrease the size
	}
	void pop_back(){//!< remove elements from the tail
		if (tail_== head_ ) return;// empty			
		tail_=previous(tail_);// advance the tail
		--Size;// decrease the size
	}
	void resize(unsigned int growto){
		if(growto <= Capacity) return;// dont allow shrinking
		T* tmp = (T*)malloc( sizeof(T)*growto);
		size_t slotstocopy;
		T **head, **tail;
		if(head_ > tail_){// the header is greater than the tail, get the bytes to the tail
			head = &head_;
			tail = &tail_;
		} else {
			head = &tail_;
			tail = &head_;
		}
		slotstocopy = *tail- ring_ +1;// include the tail in the copy
		*head= (tmp + growto) - (Capacity - (*head - ring_) ) ;// dont go past it
		*tail = tmp + slotstocopy -1;// dont go past it	
		memcpy(tmp, ring_, slotstocopy*sizeof(T));// copy over the the first part of the ring buffer. 
		memcpy(*head, ring_ + slotstocopy, (Capacity - slotstocopy)*sizeof(T));// skip the center because that is the newly added area
		free(ring_);
		Capacity=growto;
		ring_=tmp;
	}
	bool empty() { return Size ==0 ; }
	/*
	// currently not working. I dont need this function right now, so I am holding on it 
	void erase(T* element){
		if(element < ring_ || element >= ring_ + Capacity || empty()) return;// not contained within the array
		if(element ==head_) {
			head_ = previous(head_);// easy to erase that, just move back an element
			std::cout<<"element ==head_"<<std::endl;
			return;
		}
		if(element ==tail_){
			tail_ = next(tail_);// easy to erase that, just move forward an element
			std::cout<<"element ==tail_"<<std::endl;
			return;
		}
		if(head_ > tail_){// the elements are between head and tail
			if(element - tail_ < head_ - element){// the elements between the tail and element are less than the ones between the head and element
				do{// shift the array down
					*element = *(element-1);
				}while(--element != tail_-1);
				tail_+=1;// move the tail up one
			} else {
				do{// shift the array up
					*element = *(element+1);
				}while(++element != head_+1);
				head_-=1;
			}
		} else {// tail is greater than head
			if(element < tail_){
				unsigned int taildist = tail_ -ring - Capacity + element - ring_;
				unsigned int headdist = head_ - element;
				if(taildist < headdist){// distance from the tail to the element is less than the head dist
					do{// shift the array down
						*element = *(element-1);
					}while(--element != ring_-1);
					*ring=ring_[Capacity-1];
					elemet = tail_;
					do{// shift the array down
						*element = *(element+1);
					}while(++element != Capacity-1);
					tail_ = next(tail_);
					do{// shift the array down
						*element = *(element-1);
					}while(--element != ring_-1);




				}
				if(element - tail_ < head_ - element){// the elements between the tail and element are less than the ones between the head and element
					do{// shift the array down
						*element = *(element-1);
					}while(--element != tail_-1);
					tail_+=1;// move the tail up one
				} else {
					do{// shift the array up
						*element = *(element+1);
					}while(++element != head_+1);
					head_-=1;
				}
			}
			if(element - tail_ < head_ - element){// the elements between the tail and element are less than the ones between the head and element
				do{// shift the array down
					*element = *(element-1);
				}while(--element != tail_-1);
				tail_+=1;// move the tail up one
			} else {
				do{// shift the array up
					*element = *(element+1);
				}while(++element != head_+1);
				head_-=1;
			}
		}
	}
	*/
	unsigned int size(){ return Size; }
	unsigned int capacity() { return Capacity; }
	T* begin(){ return head_; }
	T* end(){ return tail_;}
	T* next(T* current) {return  (current >= ring_ + Capacity ? ring_ : current+1);}
	T* previous(T* current) {return  (current <= ring_ ? ring_ + Capacity- 1 : current-1);}
	RingBuffer() : ring_((T*)malloc(sizeof(T)*16) ), Capacity(16), Size(0){ head_= tail_=ring_;}
	~RingBuffer() { free(ring_); }
private:
	unsigned int Size, Capacity;
	T *ring_, *head_, *tail_;
};
#endif
