#ifndef MY_SIGNAL_H
#define MY_SIGNAL_H
#include <vector>
#include <map>
#include <atomic>
#include <mutex>
#include <functional>


namespace MY_Utilities{

	template <typename rettype, typename... T> class Signal_st{
	public:
		Signal_st(): _Is_Calling(false){}
		~Signal_st(){ Disconnect_All(); }

		void Disconnect_All(){
			for(auto i=_Connections.begin(); i != _Connections.end(); i++) {
				(*i)->_Connections.erase(this); (*i)->_Slots.erase(this); 
			}  
			_Connections.clear(); _Slots.clear(); 
		}
		void Disconnect(Signal_st<rettype, T...>* othersig){  othersig->_Connections.erase(this); othersig->_Slots.erase(this);  _Connections.erase(othersig); _Slots.erase(othersig);  } 
		void Connect(Signal_st<rettype, T...>* othersig, std::function<rettype(T...)> f){  
			auto i = _Slots.find(othersig);
			if(i!=_Slots.end()) (*i).second=f;
			else _Slots[othersig]=f;
			_Connections.insert(othersig);
			othersig->_Connections.insert(this);
		}
		void Call(T... params){
			if(_Is_Calling) return;//avoid infinite recursion
			_Is_Calling = true;
			for(auto i=_Slots.begin(); i!= _Slots.end(); i++){
				if((*i).first->Is_Calling()) continue;
				(*i).second(params...);
			}
			_Is_Calling = false;
		}
		bool Is_Calling() const { return _Is_Calling;}
	protected:

		std::map<Signal_st<rettype, T...>*, std::function<rettype(T...)>> _Slots;
		std::set<Signal_st<rettype, T...>*> _Connections;
		bool _Is_Calling;
	};

	template <typename rettype, typename... T> class Signal_mt{
	public:
		Signal_mt(): _Is_Calling(false){}
		~Signal_mt(){ Disconnect_All(); }
	
		void Disconnect_All(){//disconnects ALL!!
			for(auto i=_Connections.begin(); i != _Connections.end(); i++) {
				(*i)->_Lock.lock(); 
				(*i)->_Connections.erase(this); (*i)->_Slots.erase(this); 
				(*i)->_Lock.unlock(); 
			}  
			_Connections.clear(); _Slots.clear(); 
		}

		void Connect(Signal_mt<rettype, T...>* othersig, std::function<rettype(T...)> f){  
			_Lock.lock();
			auto i = _Slots.find(othersig);
			if(i!=_Slots.end()) (*i).second=f;
			else _Slots[othersig]=f;
			_Lock.unlock();
		}
		void Disconnect(Signal_mt<rettype, T...>* othersig){ othersig->_Lock.lock(); othersig->_Connections.erase(this); othersig->_Slots.erase(this); othersig->_Lock.unlock();  _Lock.lock();  _Connections.erase(othersig); _Slots.erase(othersig);  _Lock.unlock(); }

		void Call(T... params){
			if(_Is_Calling.load(std::memory_order::memory_order_acquire)) return;//avoid infinite recursion
			_Is_Calling.store(true, std::memory_order::memory_order_release);
			_Lock.lock();
			for(auto i=_Slots.begin(); i!= _Slots.end(); i++){
				if((*i).first->Is_Calling()) continue;
				(*i).second(params...);
			}
			_Lock.unlock();
			_Is_Calling.store(false, std::memory_order::memory_order_release);
		}
		bool Is_Calling() const { return _Is_Calling;}
	protected:
		std::map<Signal_mt<rettype, T...>*, std::function<rettype(T...)>> _Slots;
		std::atomic<bool> _Is_Calling;
		std::mutex _Lock;
	};
};


#endif