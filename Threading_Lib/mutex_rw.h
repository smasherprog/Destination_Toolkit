#ifndef THREADING_MUTEX_RW
#define THREADING_MUTEX_RW
#include <atomic>
#include <mutex>
#include <condition_variable>


namespace Threading{
	namespace Mutex_RW_Internal{
		class NoCopy{
			NoCopy(const NoCopy& c) {}
			NoCopy& operator=(const NoCopy& rhs){ return *this; }
		public:
			NoCopy() {}
		};
	};
	class spin_mutex_rw: public Mutex_RW_Internal::NoCopy {// no copies allowed
		// protected my privates!
		std::atomic<bool> writer_lock;
		std::atomic<unsigned int> readers;
	public:
		spin_mutex_rw(): readers(0), writer_lock(false) {}
		void lock_w(){
			while(writer_lock.exchange(true, std::memory_order::memory_order_release)){ }// wait until this thread grabs the writer lock
			while(readers.load(std::memory_order::memory_order_relaxed) != 0){ }// wait until readers == 0
		}
		void unlock_w(){
			writer_lock.store(false, std::memory_order::memory_order_release);
		}
		void lock_r(){
			while(writer_lock.load(std::memory_order::memory_order_relaxed)){}// wait until there is no writer lock
			while(readers.fetch_add(1, std::memory_order::memory_order_release)){// add one to the reader count
				if(writer_lock.load(std::memory_order::memory_order_relaxed)){// a writer grabbed the lock in between the first while loop, back the reader count down and keep waiting
					readers.fetch_sub(1, std::memory_order::memory_order_release);// decrease the reader count
				} else { return; }// otherwise, the writer lock is still false, go with READ
			}
			
		}
		void unlock_r(){
			readers.fetch_sub(1, std::memory_order::memory_order_release);// decrease the reader count
		}
	};



	class mutex_rw: public Mutex_RW_Internal::NoCopy {// no copies allowed
		std::mutex writer_lock;
		std::atomic<unsigned int> readers;
		std::atomic<bool> writer_waiting;

	public:
		mutex_rw(): readers(0), writer_waiting(false) { }// initialize the mutex
		
		void lock_w(){
			writer_waiting.store(true, std::memory_order::memory_order_release);// fisrt set to this true so a reader can know if the writer is about to aquire a lock
			writer_lock.lock();// set the lock
			while(readers.load(std::memory_order::memory_order_acquire) != 0 ){}// wait till all readers are done
		}
		void unlock_w(){
			writer_lock.unlock();
			writer_waiting.store(false, std::memory_order::memory_order_release);
		}
		void lock_r(){
			while(true){
				std::unique_lock<std::mutex> lock(writer_lock, std::defer_lock_t());// if a writer has locked the mutex, this will wait till the lock is released.
				unsigned int t = readers.fetch_add(1, std::memory_order::memory_order_release);// increment the reader count, 
				if(writer_waiting.load(std::memory_order::memory_order_acquire)){// if a writer lock is being acquired, retry getting the lock
					readers.fetch_sub(1, std::memory_order::memory_order_release);// back the count back down 
				} else return;// else the read lock was good.. get out
			}
		}
		void unlock_r(){
			readers.fetch_sub(1, std::memory_order::memory_order_release);
		}
	};
	template<class T>class lock_guard : public Mutex_RW_Internal::NoCopy{// no copies allowed
		T& mutex;
		bool reader;
	public:
		lock_guard(T& m, bool reading = true): mutex(m), reader(reading) {
			if(reader){ mutex.lock_r();
			} else { mutex.lock_w(); }
		}
		~lock_guard(){
			if(reader){ mutex.unlock_r();
			} else { mutex.unlock_w(); }
		}
	};

};
#endif