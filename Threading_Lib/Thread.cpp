#include "stdafx.h"
#include "Thread.h"

Threading::Internal_Use::cThread Threading::Internal_Use::Thread_Pool[64];
std::thread* Threading::Internal_Use::Threads[64];
int Threading::Internal_Use::NumOfThreads(0);
std::atomic<unsigned int> Threading::Internal_Use::ThreadInc;
unsigned int Threading::Internal_Use::ThreadIds[64];
bool Threading::Internal_Use::ThreadingStarted(false);

size_t Threading::Base_Task_Work::Add_Child(std::function<void ()>& work){// this is not thread-safe. Only a single thread can call this at once.....
	std::shared_ptr<Internal_Use::Task_Work> obj;
	Children.push_back(obj);// copy in as a weak_ptr
	return Threading::RunTask(work, (std::shared_ptr<Base_Task_Work>)obj);// start work immediatly and return the thread id it was assigned to
}
void Threading::Base_Task_Work::Wait(){
#ifndef THREADING
	return;
#else
	while(Status.load(std::memory_order::memory_order_acquire) != FINISHED){
		std::this_thread::yield();
	}// while the work is not completed, yeild the rest of this timeslice
#endif
}
void Threading::Base_Task_Work::WaitForAll(){// waits on the children only. This is not thread-safe, but it should only be called by a single thread at a time. Having different threads call this could potentially be bad because the function doesnt just spin, but actually does work
	// run through all the children to help out if possible
	unsigned int z=0;
	do{
		auto i =Children.begin();
		while(i !=Children.end()){
			std::shared_ptr<Base_Task_Work> ptr(*i);// must be convered to a shared_ptr to use 
			if(ptr.get() != nullptr){// check if the object was deleted between statements
				unsigned int expected = NOT_STARTED;
				if( ptr->Status.compare_exchange_strong(expected, STARTED)){// if the task has not been started, work on it
					ptr->execute();
					if(ptr->cb != 0) (ptr->cb)();
					ptr->Status.store(FINISHED, std::memory_order::memory_order_release);
				} else if(expected == FINISHED){// removed the work, its done
					i = Children.erase(i);// erase the object it no longer exists, delete it
					continue;
				} else {// work not done... keep waiting on the object goto next
					++i;// goto the next object
					continue;
				}
			} 

			i = Children.erase(i);// erase the object it no longer exists, delete it
		}
	} while(!Children.empty());// continue until all the children are removed from the buffer either because the work is done, or they are expired
	if(Status.load(std::memory_order::memory_order_acquire) != NOT_STARTED) Wait();
	Status.store(FINISHED, std::memory_order::memory_order_release);
}

void Threading::Internal_Use::cThread::operator()(){
	ContinueRunning=true;
	unsigned int expected = NOT_STARTED;
	do{// main loop will continue until the master thread shuts it down by setting the ContinueRunning to false		
		std::shared_ptr<Base_Task_Work> twork;
		expected = NOT_STARTED;// expected will change based on if the compare and exchange is successful or not
		if(!Task_Work_Buffer.pop(twork)){// if this returns false, try to steal work. Otherwise, the task will be poped, the  loop will restart and the work will execute
			for(int i(0); i<NumOfThreads; i++){// iterate over all threads checking for work
				if(Thread_Pool[i].Task_Work_Buffer.pop(twork)) {
					break;// found work!!! Wooho	
				}
			}
		} 
		if(twork.get() !=0){// work to do!!!
			InFlight.store(true, std::memory_order::memory_order_release);

			if( twork->Status.compare_exchange_strong(expected, STARTED)){
				twork->execute();
				std::function<void ()>& func = twork->GetCallBack();
				if(func != 0) (func)();
				twork->Status.store(FINISHED, std::memory_order::memory_order_release);
			} 
			InFlight.store(false, std::memory_order::memory_order_release);
		} else {// no work to do, wait
			InFlight.store(false, std::memory_order::memory_order_release);
			std::unique_lock<std::mutex> lk(Waiter);
			Notifier.wait(lk);// this effectively shuts the thread down until another thread adds work and wakes this thread up
		}
	}while(ContinueRunning);
}


void Threading::Internal_Use::InitalizeThreads(){//This is for interneal use only. 
	OUTPUT_DEBUG_MSG("Starting threading Library");
#ifndef THREADING
	std::cout<<"Threading library not initialized because the Preprocessor define THREADING was not defined. "<<std::endl<<"define THREADING inside the preprocessor define to start the threading library correctly."<<std::endl;
#else
	if(ThreadingStarted) return;
	NumOfThreads = std::thread::hardware_concurrency()-1;// dont forget the main thread is running too, dont oversubscribe
	if(NumOfThreads <=0 ) NumOfThreads =1;
	// create thread ids
	int thrd =0;
	for(unsigned int i=0; i< 64; i++){
		if(thrd >= NumOfThreads-1) thrd=0;
		ThreadIds[i] = thrd++;
	}

	ThreadInc=0;
	for(int i=0; i< NumOfThreads; i++){// create a thread for each core. Note, this will include hyperthreaded cores as well
		Threads[i] = new std::thread(std::ref(Thread_Pool[i]));// IMPORTANT TO PASS with std::ref, otherwise, thread will copy what is passed, which means it will copy itself and thats not GOOD
		Thread_Pool[i].IndexToSelfInArray =i;
	}
	ThreadingStarted = true;
	OUTPUT_DEBUG_MSG("Thread library initialized with "<<NumOfThreads<<" threads");
#endif
}
void Threading::Internal_Use::ShutDownThreads(){//This is for interneal use only. 
	if(!ThreadingStarted) return;
	OUTPUT_DEBUG_MSG("Shutting down threading library" );
	for(int i=0; i< NumOfThreads; i++){
		Thread_Pool[i].ContinueRunning=false;// manually stop all of the threads so I can free their memory
		Thread_Pool[i].Notifier.notify_one();// wake the thread in case it was sleeping
	}
	for(int i=0; i< NumOfThreads; i++){
		OUTPUT_DEBUG_MSG("Beginning shut down of thread: "<<i);
		Threads[i]->join();// manually stop all of the threads so I can free their memory
		delete Threads[i];
		OUTPUT_DEBUG_MSG("Finished shut down of thread: "<<i);
	}
	ThreadingStarted = false;
	OUTPUT_DEBUG_MSG("Finished shutting down the threading library. Have a nice day!");
}

