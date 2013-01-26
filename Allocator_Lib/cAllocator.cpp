#include "stdafx.h"
#include "cAllocator.h"

bool NLD_Allocator::Internal::Allocator_Started= false;
size_t NLD_Allocator::Internal::MemCapacity(0);
void* *NLD_Allocator::Internal::Mem=0;
size_t NLD_Allocator::Internal::MemSlotsSize(0), NLD_Allocator::Internal::MemSlotsCapacity(0);
unsigned int *NLD_Allocator::Internal::MemSlots=0;

size_t NLD_Allocator::Memory_In_Pool=0;
size_t NLD_Allocator::Memory_Allocated=0;
unsigned int NLD_Allocator::Internal::LastBinChecked=0;

RingBuffer<NLD_Allocator::Internal::ctmem> NLD_Allocator::Internal::zeroindex, NLD_Allocator::Internal::oneindex, NLD_Allocator::Internal::twoindex, NLD_Allocator::Internal::threeindex, NLD_Allocator::Internal::fourindex, 
	NLD_Allocator::Internal::fiveindex, NLD_Allocator::Internal::sixindex, NLD_Allocator::Internal::sevenindex, NLD_Allocator::Internal::eightindex, NLD_Allocator::Internal::nineindex, NLD_Allocator::Internal::tenindex, 
	NLD_Allocator::Internal::elevenindex, NLD_Allocator::Internal::twelveindex, NLD_Allocator::Internal::thirteenindex, NLD_Allocator::Internal::fourteenindex, NLD_Allocator::Internal::fifteenindex, NLD_Allocator::Internal::sixteenindex, 
	NLD_Allocator::Internal::seventeenindex, NLD_Allocator::Internal::eighteenindex, NLD_Allocator::Internal::nineteenindex, NLD_Allocator::Internal::twentyindex;

RingBuffer<NLD_Allocator::Internal::ctmem>* NLD_Allocator::Internal::Bins[NUMBEROFBINS]= {
	&NLD_Allocator::Internal::zeroindex, &NLD_Allocator::Internal::oneindex, &NLD_Allocator::Internal::twoindex, &NLD_Allocator::Internal::threeindex, &NLD_Allocator::Internal::fourindex, 
	&NLD_Allocator::Internal::fiveindex, &NLD_Allocator::Internal::sixindex, &NLD_Allocator::Internal::sevenindex, &NLD_Allocator::Internal::eightindex, &NLD_Allocator::Internal::nineindex, &NLD_Allocator::Internal::tenindex, 
	&NLD_Allocator::Internal::elevenindex, &NLD_Allocator::Internal::twelveindex, &NLD_Allocator::Internal::thirteenindex, &NLD_Allocator::Internal::fourteenindex, &NLD_Allocator::Internal::fifteenindex, &NLD_Allocator::Internal::sixteenindex, 
	&NLD_Allocator::Internal::seventeenindex, &NLD_Allocator::Internal::eighteenindex, &NLD_Allocator::Internal::nineteenindex, &NLD_Allocator::Internal::twentyindex
};
