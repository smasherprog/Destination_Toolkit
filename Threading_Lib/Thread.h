#ifndef THREADS_H
#define THREADS_H

/*
*
* Threading 1.0
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
* 4. This License is good for the current version of this Library which 
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


#include "assert.h"
#include <iostream>
#include <thread>
#include "MY_concurrent_queue.h"
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>

#define THREADING 1
#define DEBUG_THREADING 1
#if defined(DEBUG_THREADING)
#ifndef OUTPUT_DEBUG_MSG // nothing special about this, I use the console for standard debug info
#define OUTPUT_DEBUG_MSG(x) {												\
	std::cout<<x<<std::endl;													     	\
	}
#endif
#else
#ifndef OUTPUT_DEBUG_MSG
#define OUTPUT_DEBUG_MSG(x) {}
#endif
#endif 

namespace Threading{
	class Base_Task_Work;
}
/*! \namespace Threading
This was created with a specific mindset of speed. The more speed the less features included... generally.
The only functions that a user should call are the ones in the Threading namespace, not the Internal_Use
*/
namespace Threading{
	/*!
	If you wish to run a single task, you must derive your class from this Task_Work class and overide the virtual execute function
	*/
	enum Work_Status{ NOT_STARTED=0, STARTED, FINISHED };
	class Base_Task_Work{// all classes that want to run tasks must derive from this class and override the virtual operator execute
		Base_Task_Work(const Base_Task_Work& obj);//!< this should not be copied around. If you need to move a task around and hold onto it, use new and move the pointer around
		Base_Task_Work& operator=(Base_Task_Work& rhs); //!< this should not be copied around. If you need to move a task around and hold onto it, use new and move the pointer around
		std::condition_variable notifier;
		std::mutex mut;
		std::function<void ()> cb;// called when execute is finished 

	public:
		Base_Task_Work() {Status= NOT_STARTED; cb=std::function<void()>(); }
		virtual ~Base_Task_Work() {}// needed for derived classes
		virtual void execute() =0; //!< all classes must derive this

		void SetCallBack(std::function<void ()>& f){ cb = f;}
		std::function<void ()>& GetCallBack(){ return cb; }
		size_t Add_Child(std::function<void ()>& work);// this is not thread-safe. Only a single thread can call this at once.....	
		void Wait();
		void WaitForAll();// waits on the children only. This is not thread-safe, but it should only be called by a single thread at a time. Having different threads call this could potentially be bad because the function doesnt just spin, but actually does work
		unsigned int GetWorkStatus(){ return Status.load(std::memory_order::memory_order_acquire); }
		size_t GetChildCount(){ return Children.size(); }// this is not atomic so it might not be completely accurate, but getting the size of anything in a multi threaded environment is the same way since after the value is returned another thread can change the size so making this atomic doesnt make sense
		
		std::atomic<unsigned int> Status;
		std::vector<std::weak_ptr<Base_Task_Work>> Children;
	};

	namespace Internal_Use{
		class Task_Work: public Base_Task_Work{
		public:
			Task_Work( std::function<void ()>& f):  function(f) {  }
			virtual ~Task_Work() { }
			virtual void execute() { function(); }
			std::function<void ()> function;
		};
		class parallelfor_class: public Base_Task_Work{
		public:
			parallelfor_class(std::atomic<int>& ref, std::function<void (unsigned int, unsigned int)>& f): RefCounter(ref), function(f) {  }
			virtual ~parallelfor_class() { }
			virtual void execute() { function(beg, end); RefCounter.fetch_sub(1, std::memory_order::memory_order_release);}

			unsigned int beg, end;
			std::function<void (unsigned int, unsigned int)>& function;
			std::atomic<int>& RefCounter;
		};


		class cThread{
		public:
			cThread():ContinueRunning(false), IndexToSelfInArray(0), InFlight(0) { }
			~cThread(){
				/// no work needs to be deleted because all work is held in shared_ptr structures which free memory automatically
			}	
			void operator()();

			unsigned int IndexToSelfInArray;
			std::atomic<bool> InFlight;// if this is true, it means work is being done
			std::mutex Waiter;
			std::condition_variable Notifier;
			concurrent_containers::concurrent_queue_unbounded_mpmc<std::shared_ptr<Base_Task_Work>> Task_Work_Buffer;
			bool ContinueRunning;
		};
	
		extern cThread Thread_Pool[64];
		extern std::thread* Threads[64];
		extern unsigned int ThreadIds[64];
		extern int NumOfThreads;//LastWorkAdded is the index to the last thread that work was added. I use a round robbin approach to adding work to threads.
		extern std::atomic<unsigned int> ThreadInc;
		extern bool ThreadingStarted;

		void InitalizeThreads();
		void ShutDownThreads();
	
		inline unsigned int NextThread() { return ThreadIds[(ThreadInc.fetch_add(1, std::memory_order::memory_order_acquire))&(64-1)]; } // ThreadInc can increasse as much as it wants and wrap back, we only extract the first 5 bits to use to index 
		inline unsigned int push(const std::shared_ptr<Base_Task_Work>& value){ unsigned int id=NextThread(); Thread_Pool[id].Task_Work_Buffer.push(value); Thread_Pool[id].Notifier.notify_one(); return id; }// this wakes up the thread if its sleeping
		inline unsigned int push(const std::shared_ptr<Base_Task_Work>& value, unsigned int slot ){ unsigned int id=slot&(64-1); Thread_Pool[id].Task_Work_Buffer.push(value); Thread_Pool[id].Notifier.notify_one(); return id; }// this wakes up the thread if its sleeping

		// this class is solely used to initialize the threading library. Include this in your int main() at the begining, as soon as it goes out of scope, the destructor will shut everything down
		class cThreadStarter{
		public:
			cThreadStarter(){ Threading::Internal_Use::InitalizeThreads(); }
			~cThreadStarter(){ Threading::Internal_Use::ShutDownThreads(); }
		};	
	};

	/*! 
	This will wait on the thread until it has complete its work. This is faster than WaitForAll() since it does not wiat on all threads, just one. The id is NOT!!! the same as retrived from std::this_thread::get_id, Use the 
	*/
	inline void Wait(const int waitonthreadid){
#ifndef THREADING
	return;
#else
		assert(waitonthreadid < Internal_Use::NumOfThreads);
		assert(Threading::Internal_Use::ThreadingStarted);
		std::shared_ptr<Base_Task_Work> work;
		while(Internal_Use::Thread_Pool[waitonthreadid].Task_Work_Buffer.pop(work)) {
			unsigned int expected = NOT_STARTED;
			if( work->Status.compare_exchange_strong(expected, STARTED)){
				work->execute();
				std::function<void ()>& func = work->GetCallBack();
				if(func != 0) (func)();
				work->Status.store(FINISHED, std::memory_order::memory_order_release);
			}
		}
#endif
	}

	/*! 
	Will wait for all threads to complete work
	*/
	inline void WaitForAll(){
#ifndef THREADING
		return;
#else	

		assert(Threading::Internal_Use::ThreadingStarted);
		// this thread will not just spin waiting, it will execute work as well
		std::shared_ptr<Base_Task_Work> work;
		for(int i(0); i < Threading::Internal_Use::NumOfThreads; i++){// for each thread
			while(Internal_Use::Thread_Pool[i].Task_Work_Buffer.pop(work)) {
				unsigned int expected = NOT_STARTED;
				if( work->Status.compare_exchange_strong(expected, STARTED)){
					work->execute();
					std::function<void ()>& func = work->GetCallBack();
					if(func != 0) (func)();
					work->Status.store(FINISHED, std::memory_order::memory_order_release);
				}
			}
		}
		// the above code helps the threads out as much as possible with work, but eventually we have to get to a loop where we just spin waiting for the last bit of work to finish
		bool continuewaiting = false;
		do{// keep waiting until all the threads are done with work
			continuewaiting = false;
			for(int i(0); i < Threading::Internal_Use::NumOfThreads; i++){// for each thread
				continuewaiting |=Internal_Use::Thread_Pool[i].InFlight.load(std::memory_order::memory_order_relaxed);
			}
			
		}while(continuewaiting);
	#endif
	}	
	/*!
	RunTask below maps the task to a specific thread specified by slot. If you have 7 threads allocated, you can call the function with a slot [0, 7]   so   RunTask(work, 4);    to run the task on thread 4
	*/
	inline unsigned int RunTask(std::function<void()> work, std::shared_ptr<Base_Task_Work> obj, unsigned int slot){// this function takes a shared_ptr so that the caller can still hold the task if he/she wants 
#ifndef THREADING // for single core builds
		work();
		return 0;
#else	
		assert(Threading::Internal_Use::ThreadingStarted);
		obj = std::make_shared<Threading::Internal_Use::Task_Work>(work);
		obj->Status=NOT_STARTED;// just in case
		return Threading::Internal_Use::push(obj, slot);
#endif
	}
	inline unsigned int RunTask(std::function<void()> work, std::shared_ptr<Base_Task_Work> obj, unsigned int slot, std::function<void()> callback){// this function takes a shared_ptr so that the caller can still hold the task if he/she wants 
#ifndef THREADING // for single core builds
		work();
		return 0;
#else	
		assert(Threading::Internal_Use::ThreadingStarted);
		obj = std::make_shared<Threading::Internal_Use::Task_Work>(work);
		obj->Status=NOT_STARTED;// just in case
		obj->SetCallBack(callback);
		return Threading::Internal_Use::push(obj, slot);
#endif
	}
	inline unsigned int RunTask(std::function<void()> work, std::shared_ptr<Base_Task_Work> obj){// this function RETURNS the actual work object Base_Task_Work so the user can wait on the work to complete
#ifndef THREADING // for single core builds
		work();
		return 0;
#else	
		assert(Threading::Internal_Use::ThreadingStarted);
		obj = std::make_shared<Threading::Internal_Use::Task_Work>(work);
		obj->Status=NOT_STARTED;// just in case
		return Threading::Internal_Use::push(obj);
#endif
	}
	inline unsigned int RunTask(std::function<void()> work, std::shared_ptr<Base_Task_Work> obj, std::function<void()> callback){// this function RETURNS the actual work object Base_Task_Work so the user can wait on the work to complete
#ifndef THREADING // for single core builds
		work();
		return 0;
#else	
		assert(Threading::Internal_Use::ThreadingStarted);
		obj = std::make_shared<Threading::Internal_Use::Task_Work>(work);
		obj->Status=NOT_STARTED;// just in case
		obj->SetCallBack(callback);
		return Threading::Internal_Use::push(obj);
#endif
	}
	inline unsigned int RunTask(std::function<void()> work){
#ifndef THREADING // for single core builds
		work();
		return 0;
#else	
		assert(Threading::Internal_Use::ThreadingStarted);
		return Threading::Internal_Use::push(std::make_shared<Threading::Internal_Use::Task_Work>(work));
#endif
	}
	inline unsigned int RunTask(std::function<void()> work, unsigned int slot){
		
#ifndef THREADING // for single core builds
		work();
		return 0;
#else	
		assert(Threading::Internal_Use::ThreadingStarted);
		return Threading::Internal_Use::push(std::make_shared<Threading::Internal_Use::Task_Work>(work), slot);
#endif
	}
	inline unsigned int RunTask(std::function<void()> work, std::function<void()> callback){
		
#ifndef THREADING // for single core builds
		work();
		return 0;
#else	
		assert(Threading::Internal_Use::ThreadingStarted);
		std::shared_ptr<Base_Task_Work> obj = std::make_shared<Threading::Internal_Use::Task_Work>(work);
		obj->SetCallBack(callback);
		return Threading::Internal_Use::push(obj);
#endif
	}
	/*!
	the range is [beg, end), this will not create any copies of the work passed, it will use it as a reference.
	splitssize is size of the each split . . . work is added in a round robin style to the different threads. This function also blocks until the work is complete
	*/
	inline void Parallel_For(const unsigned int beg, const unsigned int end,  const unsigned int splitsize, std::function<void(unsigned int, unsigned int)> work){
		
#ifndef THREADING // for single core builds
			return work(beg, end);
#else 
			assert(Threading::Internal_Use::ThreadingStarted);
			unsigned int range = end - beg;
			unsigned int iterationspersplit = range / splitsize;
			std::atomic<int> counter = 0;
			int counting=0;
			if((iterationspersplit ==0) | (splitsize >= range)) {
				iterationspersplit = range;
				counting=counter=1;// one unit of work
			} else {
				if(iterationspersplit*splitsize != range){// there will be an extra work added
					counting=counter=splitsize +1;
				} else counting=counter=splitsize;
			}
			unsigned int i(beg);

			unsigned int arrindex=0;
			std::shared_ptr<Internal_Use::parallelfor_class>* arr = new std::shared_ptr<Internal_Use::parallelfor_class>[counting];

			// parallel_for lets the individual threads free the memory because some work might still be inside the work queue when this function finishes
			while(i + iterationspersplit <= end){
				arr[arrindex] = std::make_shared<Internal_Use::parallelfor_class>(counter, work);
				arr[arrindex]->beg=i;
				i+=iterationspersplit;
				arr[arrindex]->end = i;
				Threading::Internal_Use::push(arr[arrindex]);
				arrindex+=1;
			} 
			if(i < end){// add the remaining work, if any
				arr[arrindex] = std::make_shared<Internal_Use::parallelfor_class>(counter, work);
				arr[arrindex]->beg=i;
				arr[arrindex]->end = end;
				Threading::Internal_Use::push(arr[arrindex]);
				arrindex+=1;
			}
			unsigned int index=0;
			while(index < arrindex){// while the work is not completed, work on it
				unsigned int expected = NOT_STARTED;
				if( arr[index]->Status.compare_exchange_strong(expected, STARTED)){
					arr[index]->execute();
					arr[index]->Status.store(FINISHED, std::memory_order::memory_order_release);
				}
				index+=1;
			}

			while(counter.load(std::memory_order::memory_order_relaxed) > 0){
				std::this_thread::yield();// if we reach the end, it means all work is in progress and there is nothing left to do but wait
			}
			delete [] arr;

#endif	
	}
};





#endif

