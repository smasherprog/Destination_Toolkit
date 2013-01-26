#ifndef CALLOCATOR_H
#define CALLOCATOR_H

/*
 *
 * Allocator 1.0
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


#include <iostream>

#include "../Threading_Lib/RingBuffer.h"
#include "../Utilities/My_Timer.h"
#include "../Utilities/MyMath.h"
// turn debug on to do memory leak checks

#if defined(DEBUG) | defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

//#define ALLOCATOR_DEBUG 1// use this to turn off and on the allocator. With this undefined, the allocator will use the standard free and malloc, so if there is a problem .. its not my fault!
#define GROWBY 128
#define TIME_BEFORE_EVICTION 1.0  // this is defined as a double, so 1.0  is 1 seconds
#define SEARCH_SPREAD 4 //This is how many bins to check every time a deallocation occurs. The check is for memory that has not been used in a while to give it back to the OS
#define MEGABYTE 1000000 // number of bytes in a megabyte
#define MEMORY_USED_BEFORE_EVICTIONS MEGABYTE * 2// this will hold 2 megabytes in memory before evictions happen.
#define NUMBEROFBINS 21
#define NOTPOWOFTWO(a) (a && (a & (a - 1) ) )// returns false if the number is a power of two.. true otherwise
#ifndef _WIN64  // 32 bit builds do not generally guarantee a 16 byte alignment, but a 64 bit build should be aligned to 16 byte offsets
	#define USE_CUSTOM_ALIGNMENT// If this is commented out, the library will just use what the OS returns and guarantees no memory alignment. In most 64 bit builds, the OS will return memory on 16 byte allignment, so this isnt needed.
#endif
#define ALIGN_MEM_TO 16 // define the alignment, must be a power of TWOO!!!!!!
namespace NLD_Allocator{

	extern size_t Memory_In_Pool;//!< this is the current memory waiting in the pool. i.e. memory that was freed and is waiting around to be used again.  
	extern size_t Memory_Allocated;//!< this is the current memory allocated by this library. 
	namespace Internal{	
		struct ctmem{
			My_Timer::tick_count LastTimeUsed;
			void* beg;
		};
		extern RingBuffer<ctmem> zeroindex, oneindex, twoindex, threeindex, fourindex, fiveindex, sixindex, sevenindex, eightindex, nineindex, tenindex, 
			elevenindex, twelveindex, thirteenindex, fourteenindex, fifteenindex, sixteenindex, seventeenindex, eighteenindex, nineteenindex, twentyindex;
		extern RingBuffer<ctmem>* Bins[NUMBEROFBINS];
		struct cMemHeader{
			unsigned int Slot, Index;
		};
		extern bool Allocator_Started;
		extern size_t MemCapacity;
		extern void* *Mem;//!< stores all the allocated memory so when a program exits, I can delete it in the ~cAllocStarter to ensure no memory leaks occur
		extern size_t MemSlotsSize, MemSlotsCapacity;
		extern unsigned int *MemSlots;//!< Holds slot ids to the Mem vector
		extern unsigned int LastBinChecked;//!< used for evicting memory
		/*!
		 When this starts, it creates some initial ID'S, and when it destroys itself at program exit, it frees all the memory even if the user forgets to, thus ensuring no memory leaks
		*/
		class cAllocStarter{
		public:
			cAllocStarter() { //!< This created some IDs on start up
				Allocator_Started= true;
				MemSlots = (unsigned int *)malloc(sizeof(unsigned int) * GROWBY);
				MemSlotsCapacity = GROWBY;
				MemSlotsSize = GROWBY-1;
				for(unsigned int i(0); i< GROWBY; i++)
					MemSlots[i]=i;
				Mem = (void**) malloc(sizeof(void*) * GROWBY);
				memset(Mem, 0, sizeof(void*) * GROWBY);
				MemCapacity = GROWBY;
#if defined(DEBUG) | defined(_DEBUG)
				_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
			}
			~cAllocStarter() {
				Allocator_Started=false;
				for(size_t i(0); i< MemCapacity; i++) {
					if(Mem[i] !=0) free(Mem[i]);
				}
				free(Mem);
				free(MemSlots);
			}
		};
		

		inline unsigned int GetMemSlot(){//< Gets a valid slot to place the allocated memory
			if(MemSlotsSize ==0){// create more ids
				unsigned int id = MemSlots[0];// get the last id
				void* tmp = malloc(  (GROWBY *sizeof(unsigned int)) + (MemSlotsCapacity * sizeof(unsigned int)) );
				free(MemSlots);
				MemSlots = (unsigned int*)tmp;
				for(size_t i(MemSlotsCapacity), j(0); i< MemSlotsCapacity + GROWBY; i++, j++){// this means memslots is empty so I can insert new ids at the very beginning of the array
					MemSlots[j]=(unsigned int)i;
				}
				MemSlotsCapacity += GROWBY;
				tmp = malloc((sizeof(void*) *MemCapacity) + (GROWBY *sizeof(void*) ));
				memcpy(tmp, Mem, sizeof(void*) *MemCapacity);
				memset((char*)tmp + sizeof(void*) *MemCapacity, 0,  GROWBY *sizeof(void*));
				free(Mem);
				Mem=(void**)tmp;
				MemCapacity+= GROWBY;
				MemSlotsSize=GROWBY-1;// this is an index to the end of the memory slots, inserts and removals all happen at the end of the array, hence the -1
				return id;
			}
			return MemSlots[MemSlotsSize--];
		}	
		/*!
			WHY is this function named ahel? I dont know, but I like it. Its job is to find memory and return it. 
		*/
		inline void* ahel(const unsigned int size, RingBuffer<ctmem>& cont, const unsigned int index) {	
			if(!cont.empty()) {	
				
				void* tmp(cont.front().beg);	
				cont.pop_front();
				Memory_In_Pool-=size + sizeof(Internal::cMemHeader);

				return tmp;													
			}	

#if defined(USE_CUSTOM_ALIGNMENT)
			void *p1; // basic pointer needed for computation.
			p1 = malloc(size + ALIGN_MEM_TO + sizeof(Internal::cMemHeader) );// allocate extra space for our header and alignment requirements		
			if(p1==NULL) return NULL;
			/*
			assuing MEMALIGNTO = 16
			MEMALIGNTO -1 = 15 = 0000 1111
			~15 = 1111 0000
			assuimg p1 = 1010 1100  it doesnt matter what the address is
			p1 + MEMALIGNTO = 1011 1100
			p2 = p1 & ~15
			   1011 1100 
            &  1111 0000
			p2=1011 0000 
			*/
			// size_t is always the same size as a pointer so operations on it are the same as pointer arithmatic. I use a size_t because you cannot do some bitwise
			// functions on pointers, but only integer types. Convert it back to a pointer on return
			char* p2= (char*) ( ( (size_t)( (char*)p1+ALIGN_MEM_TO) ) & ( ~(ALIGN_MEM_TO-1) ) );// p2 is now the aligned memory address to return
			Internal::cMemHeader* header = reinterpret_cast<Internal::cMemHeader*>(p2 - sizeof(Internal::cMemHeader));
			Memory_In_Pool+=size + ALIGN_MEM_TO+ sizeof(Internal::cMemHeader);  
#else 
			// in this case, the allocator assumes that memory from malloc is aligned to 16 bytes
			size_t sizetoalloc = size + (ALIGN_MEM_TO - sizeof(Internal::cMemHeader)) + sizeof(Internal::cMemHeader);
			char* p1 = (char*)malloc(sizetoalloc);// allocate extra space for our header and alignment requirements
			Memory_Allocated+=sizetoalloc;  
			Internal::cMemHeader* header = reinterpret_cast<Internal::cMemHeader*>(p1 + ALIGN_MEM_TO - sizeof(Internal::cMemHeader));
			Memory_In_Pool+=sizetoalloc;
			char* p2 = p1 + ALIGN_MEM_TO; 

#endif
			header->Slot=Internal::GetMemSlot();								
			header->Index=index;											 
			Internal::Mem[header->Slot] = p1;						
			return p2;					
		} 
		/*!
			This function checks other bins to see if some of the memory is too old and should be released back to the OS. Check the MEMORY_USED_BEFORE_EVICTIONS. If you want, you can set this
			to something higher, like 5 MB so you will be guaranteed a nice sized pool to ensure fast allocations. Remember, evictions happen to the memory chunks that have been sitting in the buffer the longest
			Memory is not evicted if it was recently used. Use TIME_BEFORE_EVICTION to set the time allowed for memory to sit untouched before eviction. However, this will only occur if the memory pool 
			is bigger than MEMORY_USED_BEFORE_EVICTIONS, otherwise, it doesnt matter hold long it has been since the memory has been used, it will stay in the pool until the threshold is hit.
		*/
		inline void CheckForEvictions(unsigned int index, size_t size){
			if(MEMORY_USED_BEFORE_EVICTIONS > Memory_In_Pool) return;// enough memory has not been allocated to start evictions
			
			size_t counter(0);
			My_Timer::tick_count time = My_Timer::tick_count::now();
			unsigned int nextbin = index;
			do{			
				if(Bins[nextbin]->empty()) continue;//this bin is empty
				RingBuffer<ctmem>& bin(*Bins[nextbin]);// easier access this way
				ctmem* end(bin.end());
				ctmem* beg(bin.begin());
				if(++Internal::LastBinChecked > NUMBEROFBINS-1) Internal::LastBinChecked=0;
				nextbin = Internal::LastBinChecked;
				do{		
					end = bin.previous(end);
					Internal::cMemHeader* header= reinterpret_cast<Internal::cMemHeader*>((char*)end->beg - sizeof(Internal::cMemHeader));
					unsigned int slot = header->Slot;
					if((time - end->LastTimeUsed).seconds() >=TIME_BEFORE_EVICTION){	
						void* ch =Internal::Mem[slot];
						Memory_In_Pool-=size + sizeof(Internal::cMemHeader);
						Memory_Allocated -=size + sizeof(Internal::cMemHeader);
						free(ch);// the memory has been sitting for too long not used, remove it
						Internal::Mem[slot] =0;// make sure to set this to zero.. calling delete on a pointer of zero is ok in c++
						Internal::MemSlots[++MemSlotsSize]=slot;// insert the slot now to be used for future
						Bins[index]->pop_back();// make sure to remove this node as well		
					} else {
						break;// done with this bin, the rest have to be recently used
					}
				}while(end!=beg);
			}while(++counter != SEARCH_SPREAD);
		}
	};
	/*!
		this will search the pool for the requested size and return the memory. When in debug mode, the library switches to the standard malloc. 
	*/
	inline void* NLD_malloc(unsigned int len){// if you need more than 4 gigs allocation in one chunk.. GO somewhere else!!!! YOU ARE CRAZY!
		assert(Internal::Allocator_Started);
		//std::cout<<"Allocating "<<len<<" Bytes. ";
#if defined(ALLOCATOR_DEBUG)
		return malloc(len);
#endif
		unsigned int index(0);
		index = BSR(len);// get the index of the highest bit set
		index+= NOTPOWOFTWO(len);// this will be zero or one .. no branching!
		//std::cout<<"Highest bit set "<<index<<" final bitset "<<index<<std::endl;
		void* tmp;
		switch(index){
			// this is commented out on purpose. I get a speed boost by skipping the first three checks. Allocations that small shouldnt happen that frequently anyway
			// if you want to un comment these, make sure to uncomment the opposite ones in the call to free
			case(0):
				//return Internal::ahel(1, Internal::zeroindex, index);
			case(1):
				//return Internal::ahel(2, Internal::oneindex, index);
			case(2):
				//return Internal::ahel(4, Internal::twoindex, index);
			case(3):
				//return Internal::ahel(8, Internal::threeindex, index);
			case(4):
				return Internal::ahel(16, Internal::fourindex, index);
			case(5):
				return Internal::ahel(32, Internal::fiveindex, index);
			case(6):
				return Internal::ahel(64, Internal::sixindex, index);
			case(7):
				return Internal::ahel(128,Internal::sevenindex, index);
			case(8):
				return Internal::ahel(256, Internal::eightindex, index);
			case(9):
				return Internal::ahel(512, Internal::nineindex, index);
			case(10):
				return Internal::ahel(1024, Internal::tenindex, index);
			case(11):
				return Internal::ahel(2048, Internal::elevenindex, index);
			case(12):
				return Internal::ahel(4096, Internal::twelveindex, index);
			case(13):
				return Internal::ahel(8192, Internal::thirteenindex, index);
			case(14):
				return Internal::ahel(16384, Internal::fourteenindex, index);
			case(15):
				return Internal::ahel(32786, Internal::fifteenindex, index);
			case(16):
				return Internal::ahel(65536, Internal::sixteenindex, index);
			case(17):
				return Internal::ahel(131072, Internal::seventeenindex, index);
			case(18):
				return Internal::ahel(262144, Internal::eighteenindex, index);
			case(19):
				return Internal::ahel(524288, Internal::nineteenindex, index);
			case(20):
				return Internal::ahel(1048576, Internal::twentyindex, index);
			default:// use the standard memory allocator after this
				
				tmp = malloc(len + sizeof(sizeof(Internal::cMemHeader)) );
				reinterpret_cast<Internal::cMemHeader*>(tmp)->Index = 30;
				return (char*)tmp +sizeof(Internal::cMemHeader);
		};
	}
	/*!
		The memory will be inserted back into the pool and a check is made whether evictions should start occuring. The memory will not be actually released until some criteria are met. Check out 
		Internal::CheckForEvictions for the criteria. When in debug mode, the library switches to the standard free. 
	*/
	inline void NLD_free(void* ptr){	
		if(ptr == 0) return;
		assert(Internal::Allocator_Started);
#if defined(ALLOCATOR_DEBUG)
		return free(ptr);
#endif
		Internal::cMemHeader* header= reinterpret_cast<Internal::cMemHeader*>((char*)ptr - sizeof(Internal::cMemHeader));		
		unsigned int index = header->Index;
		Internal::ctmem t;
		t.LastTimeUsed =My_Timer::tick_count::now();
		t.beg=ptr;
		switch(index){
			// these are commented out on purpose. Check out malloc for the reason why
			case(0):
				/*
				Internal::zeroindex.push_front(t);
				Memory_In_Pool+= 1 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(0, 1);
				break;
				*/
			case(1):
				/*
				Internal::oneindex.push_front(t);
				Memory_In_Pool+= 2 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
	
				Internal::CheckForEvictions(1, 2);
				break;
				*/
			case(2):
				/*
				Internal::twoindex.push_front(t);
				Memory_In_Pool+= 4 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(2, 4);
				break;
				*/
			case(3):
				/*
				Internal::threeindex.push_front(t);
				Memory_In_Pool+= 8 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(3, 8);
				break;
				*/
			case(4):
				Internal::fourindex.push_front(t);
				Memory_In_Pool+= 16 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(4, 16);
				break;
				
			case(5):
				Internal::fiveindex.push_front(t);
				Memory_In_Pool+= 32 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(5, 32);
				break;
				
			case(6):
				Internal::sixindex.push_front(t);
				Memory_In_Pool+= 64 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(6, 64);
				break;
			case(7):
				Internal::sevenindex.push_front(t);
				Memory_In_Pool+= 128 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(7, 128);
				break;
			case(8):
				Internal::eightindex.push_front(t);
				Memory_In_Pool+= 256 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(8, 256);
				break;
			case(9):
				Internal::nineindex.push_front(t);
				Memory_In_Pool+= 512 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(9, 512);
				break;
			case(10):
				Internal::tenindex.push_front(t);
				Memory_In_Pool+= 1024 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(10, 1024);
				break;
			case(11):
				Internal::elevenindex.push_front(t);
				Memory_In_Pool+= 2048 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(11, 2048);
				break;
			case(12):
				Internal::twelveindex.push_front(t);
				Memory_In_Pool+= 4096 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(12, 4096);
				break;
			case(13):
				Internal::thirteenindex.push_front(t);
				Memory_In_Pool+= 8192 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(13, 8192);
				break;
			case(14):
				Internal::fourteenindex.push_front(t);
				Memory_In_Pool+= 16384 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(14, 16384);
				break;
			case(15):
				Internal::fifteenindex.push_front(t);
				Memory_In_Pool+= 32768 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(15, 32768);
				break;
			case(16):
				Internal::sixteenindex.push_front(t);
				Memory_In_Pool+= 65536 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(16, 65536);
				break;
			case(17):
				Internal::seventeenindex.push_front(t);
				Memory_In_Pool+= 131072 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(17, 131072);
				break;
			case(18):
				Internal::eighteenindex.push_front(t);
				Memory_In_Pool+= 262144 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(18, 262144);
				break;
			case(19):
				Internal::nineteenindex.push_front(t);
				Memory_In_Pool+= 524288 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(19, 524288);
				break;
			case(20):
				Internal::twentyindex.push_front(t);
				Memory_In_Pool+= 1048576 + ALIGN_MEM_TO+sizeof(Internal::cMemHeader);
				Internal::CheckForEvictions(20, 1048576);
				break;
			default:// use the standard memory allocator after this			
				free((char*)ptr - sizeof(Internal::cMemHeader));
		};
	}
};

#endif