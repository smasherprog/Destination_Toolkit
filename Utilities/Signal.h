#ifndef MY_SIGNAL_H
#define MY_SIGNAL_H
#include <vector>
#include <map>
#include <functional>


namespace MY_Utilities{

	template <typename rettype, typename... T> class Signal_st{
	public:
		Signal_st(): _Is_Calling(false){}
		~Signal_st(){ clear(); }

		void clear(){//disconnects ALL!!
			for(auto i=Connections.begin(); i != Connections.end(); i++) {
				(*i)->Connections.erase(this); (*i)->Slots.erase(this); 
			}  
			Connections.clear(); Slots.clear(); 
		}
		void Disconnect(Signal_st<rettype, T...>* othersig){  othersig->Connections.erase(this); othersig->Slots.erase(this);  Connections.erase(othersig); Slots.erase(othersig);  } 
		void Connect(Signal_st<rettype, T...>* othersig, std::function<rettype(T...)> f){  
			auto i = Slots.find(othersig);
			if(i!=Slots.end()) (*i).second=f;
			else Slots[othersig]=f;
			Connections.insert(othersig);
			othersig->Connections.insert(this);
		}


		void Call(T... params){
			if(_Is_Calling) return;//avoid infinite recursion
			_Is_Calling = true;
			for(auto i=Slots.begin(); i!= Slots.end(); i++){
				if((*i).first->Is_Calling()) continue;
				(*i).second(params...);
			}
			_Is_Calling = false;
		}
		bool Is_Calling() const { return _Is_Calling;}
	protected:

		std::map<Signal_st<rettype, T...>*, std::function<rettype(T...)>> Slots;
		std::set<Signal_st<rettype, T...>*> Connections;
		bool _Is_Calling;
	};

	template <typename rettype, typename... T> class Signal_mt{
	public:
		Signal_mt(): _Is_Calling(false){}
		~Signal_mt(){ clear(); }
	
		void clear(){//disconnects ALL!!
			for(auto i=Connections.begin(); i != Connections.end(); i++) {
				(*i)->_Lock.lock(); 
				(*i)->Connections.erase(this); (*i)->Slots.erase(this); 
				(*i)->_Lock.unlock(); 
			}  
			Connections.clear(); Slots.clear(); 
		}

		void Connect(Signal_mt<rettype, T...>* othersig, std::function<rettype(T...)> f){  
			_Lock.lock();
			auto i = Slots.find(othersig);
			if(i!=Slots.end()) (*i).second=f;
			else Slots[othersig]=f;
			_Lock.unlock();
		}
		void Disconnect(Signal_mt<rettype, T...>* othersig){ othersig->_Lock.lock(); othersig->Connections.erase(this); othersig->Slots.erase(this); othersig->_Lock.unlock();  _Lock.lock();  Connections.erase(othersig); Slots.erase(othersig);  _Lock.unlock(); }

		void Call(T... params){
			if(_Is_Calling.load(std::memory_order::memory_order_acquire)) return;//avoid infinite recursion
			_Is_Calling.store(true, std::memory_order::memory_order_release);
			_Lock.lock();
			for(auto i=Slots.begin(); i!= Slots.end(); i++){
				if((*i).first->Is_Calling()) continue;
				(*i).second(params...);
			}
			_Lock.unlock();
			_Is_Calling.store(false, std::memory_order::memory_order_release);
		}
		bool Is_Calling() const { return _Is_Calling;}
	protected:
		std::map<Signal_mt<rettype, T...>*, std::function<rettype(T...)>> Slots;
		std::atomic<bool> _Is_Calling;
		std::mutex _Lock;
	};
};


#endif