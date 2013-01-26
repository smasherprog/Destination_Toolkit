#include "stdafx.h"
#include <string>

#include "../Allocator_Lib/cAllocator.h"
#include "../Utilities/My_Timer.h"

#include <vector>

using namespace std;

int main(int argc, char* argv[])
{
	NLD_Allocator::Internal::cAllocStarter AllocStarter;
	cout<<"Welcome to the allocator tester!"<<endl<<"Enter how many allocations to do (must be greater than 10) ";
	int numallocs =0;
	cin>>numallocs;
	assert(numallocs > 10);
	cout<<endl<<"Now, specify the range of allocations [min, max]. (Note: I will use rand on the range and the min must be larger than 16 bytes)"<<endl<<"Min: ";
	int beg, end;
	cin>>beg;
	cout<<"Max: ";
	cin>>end;
	assert(end > beg);
	assert(beg >= 16);
	int rndrange = end- beg;
	srand(GetTickCount());//seed random so the compiler cannot optimize away stuff
	std::vector<char*> spots(numallocs);// pre allocate enough space
	char testerstring[] = "test string";
	cout<<"<------------ Starting First round. malloc should be faster in the first round every time --------->"<<endl;

	cout<<"Starting NLD_Allocator::NLD_malloc test"<<endl;
	My_Timer::tick_count start = My_Timer::tick_count::now();
	for(int b = 0; b != numallocs; b++){
		char* p = (char*)NLD_Allocator::NLD_malloc(rand() % rndrange + beg);//this will move the allocations int [min, max] range
		spots[b] = p;
		for(int i=0; i< sizeof(testerstring); i++){
			p[i] = testerstring[i];
		}
	}
	double time1 =(My_Timer::tick_count::now()- start).seconds();
	cout<<"Finished NLD_Allocator::NLD_malloc test in "<<time1<<endl;
	// make sure to free the memory

	for(int b = 0; b != numallocs; b++){
		NLD_Allocator::NLD_free(spots[b]);//this will move the allocations int [min, max] range
	}

	cout<<"Starting malloc test"<<endl;
	start = My_Timer::tick_count::now();
	for(int b = 0; b != numallocs; b++){
		char* p = (char*)malloc(rand() % rndrange + beg);//this will move the allocations int [min, max] range
		spots[b] = p;
		for(int i=0; i< sizeof(testerstring); i++){
			p[i] = testerstring[i];
		}
	}
	double time2 =(My_Timer::tick_count::now()- start).seconds();
	cout<<"Finished malloc test in "<<time2<<endl;
	if(time1 < time2){
		cout<<"NLD_allocator was faster than malloc by "<<(time2/time1)*100.0<<"%"<<endl;
	} else {
		cout<<"malloc was faster than NLD_allocator by "<<(time1/time2)*100.0<<"%"<<endl;
	}
	// make sure to free the memory
	for(int b = 0; b != numallocs; b++){
		free(spots[b]);//this will move the allocations int [min, max] range
	}
	cout<<"<------------ Finished First round. malloc should be faster in the first round every time --------->"<<endl;
	cout<<"<------------ Starting second round of allocations test --------->"<<endl;

	cout<<"Starting NLD_Allocator::NLD_malloc test"<<endl;
	start = My_Timer::tick_count::now();
	for(int b = 0; b != numallocs; b++){
		char* p = (char*)NLD_Allocator::NLD_malloc(rand() % rndrange + beg);//this will move the allocations int [min, max] range
		spots[b] = p;
		for(int i=0; i< sizeof(testerstring); i++){
			p[i] = testerstring[i];
		}
	}
	time1 =(My_Timer::tick_count::now()- start).seconds();
	cout<<"Finished NLD_Allocator::NLD_malloc test in "<<time1<<endl;
	// make sure to free the memory
	for(int b = 0; b != numallocs; b++){
		NLD_Allocator::NLD_free(spots[b]);//this will move the allocations int [min, max] range
	}

	cout<<"Starting malloc test"<<endl;
	start = My_Timer::tick_count::now();
	for(int b = 0; b != numallocs; b++){
		char* p = (char*)malloc(rand() % rndrange + beg);//this will move the allocations int [min, max] range
		spots[b] = p;
		for(int i=0; i< sizeof(testerstring); i++){
			p[i] = testerstring[i];
		}
	}

	time2 =(My_Timer::tick_count::now()- start).seconds();
	cout<<"Finished malloc test in "<<time2<<endl;
	if(time1 < time2){
		cout<<"NLD_allocator was faster than malloc by "<<(time2/time1)*100.0<<"%"<<endl;
	} else {
		cout<<"malloc was faster than NLD_allocator by "<<(time1/time2)*100.0<<"%"<<endl;
	}
	// make sure to free the memory
	for(int b = 0; b != numallocs; b++){
		free(spots[b]);//this will move the allocations int [min, max] range
	}

	system("pause");
	return 0;
}
