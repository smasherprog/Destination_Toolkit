#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H
#include <atomic>

namespace concurrent_containers{
#define CACHE_LINE_SIZE 64 

	template<class T> class Node {// this node stores a copy of the object.
	public:
		Node(const T& val ) : value(val), next(nullptr) { }
		T value;
		std::atomic<Node<T>*> next;
	};
	// this is a multiple producer, multiple consumer queue
	template <typename T>class concurrent_queue_unbounded_mpmc {
		concurrent_queue_unbounded_mpmc<T>(concurrent_queue_unbounded_mpmc<T>& obj);
		concurrent_queue_unbounded_mpmc<T>& operator= (concurrent_queue_unbounded_mpmc<T>& objt);

	private:

		char pad0[CACHE_LINE_SIZE];

		Node<T>* first;
		char pad1[CACHE_LINE_SIZE - sizeof(Node<T>*)];

		std::atomic<bool> consumerLock;
		char pad2[CACHE_LINE_SIZE - sizeof(std::atomic<bool>)];

		Node<T>* last;
		char pad3[CACHE_LINE_SIZE - sizeof(Node<T>*)];

		std::atomic<bool> producerLock;
		char pad4[CACHE_LINE_SIZE - sizeof(std::atomic<bool>)];

	public:
		concurrent_queue_unbounded_mpmc() {
			first = last = new Node<T>(nullptr);
			producerLock = consumerLock = false;
		}
		~concurrent_queue_unbounded_mpmc() {
			while( first != nullptr ) {      // release the list
				Node<T>* tmp = first;
				first = tmp->next;
				delete tmp;
			}
		}
		void push( const T& t ) {
			Node<T>* tmp = new Node<T>(t);
			while( producerLock.exchange(true, std::memory_order_relaxed) ){ }   // acquire exclusivity
			last->next = tmp;		 // publish to consumers
			last = tmp;		 		// swing last forward
			producerLock = false; 		// release exclusivity
		}

		bool pop( T& result ) {
			while( consumerLock.exchange(true, std::memory_order_relaxed) ) { }    // acquire exclusivity
			Node<T>* theFirst = first;
			Node<T>* theNext = first->next;
			if( theNext != nullptr ) {   // if queue is nonempty
				result = theNext->value;	 // take it out
				first = theNext;		 	// swing first forward
				consumerLock = false; 	       	// release exclusivity
				delete theFirst;  	// and the old dummy
				return true;	 	// and report success
			}
			consumerLock = false;   // release exclusivity
			return false;                  // report queue was empty
		}
	};
};

#endif