#ifndef MY_SIGNAL_H
#define MY_SIGNAL_H
#include <vector>
#include <map>
#include <functional>


namespace MY_Utilities{

	template <typename rettype, typename... T> class Signal_st{
	public:
		Signal_st(): _Is_Calling(false){}
		~Signal_st(){ 
			for(auto i=Slots.begin(); i != Slots.end(); i++){
				if((*i).first == 0) continue;
				(*i).first->Disconnect(this); 
			}
		}

		void Connect(Signal_st<rettype, T...>* othersig){  Slots[othersig] = std::vector<std::function<rettype(T...)>>(0);  } 
		void Connect(Signal_st<rettype, T...>* othersig, std::function<rettype(T...)> f){  
			auto i = Slots.find(othersig);
			if(i!=Slots.end()) (*i).second.push_back(f);
			else Slots[othersig].push_back(f);
		}
		void Disconnect(Signal_st<rettype, T...>* othersig){ Slots.erase(othersig); }

		void Call(T... params){
			if(_Is_Calling) return;//avoid infinite recursion
			_Is_Calling = true;
			for(auto i=Slots.begin(); i!= Slots.end(); i++){
				if((*i).first !=0){
					if((*i).first->Is_Calling()) continue;
				}
				auto& cont = (*i).second;
				for(size_t j =0; j < cont.size(); j++){
					cont[j](params...);
				}
			}
			_Is_Calling = false;
		}
		bool Is_Calling() const { return _Is_Calling;}
	protected:
		std::map<Signal_st<rettype, T...>*, std::vector<std::function<rettype(T...)>>> Slots;
		bool _Is_Calling;
	};

	template <typename rettype, typename... T> class Signal_mt{
	public:
		Signal_mt(): _Is_Calling(false){}
		~Signal_mt(){ for(auto i=Slots.begin(); i != Slots.end(); i++) (*i).first->Disconnect(this); }

		void Connect(Signal_mt<rettype, T...>* othersig){ 
			_Lock.lock();
			Slots[othersig] = std::vector<std::function<rettype(T...)>>(0);  
			_Lock.unlock();
		} 
		void Connect(Signal_mt<rettype, T...>* othersig, std::function<rettype(T...)> f){  
			_Lock.lock();
			auto i = Slots.find(othersig);
			if(i!=Slots.end()) (*i).second.push_back(f);
			else Slots[othersig].push_back(f);
			_Lock.unlock();
		}
		void Disconnect(Signal_mt<rettype, T...>* othersig){ _Lock.lock(); Slots.erase(othersig);_Lock.unlock(); }

		void Call(T... params){
			if(_Is_Calling.load(std::memory_order::memory_order_acquire)) return;//avoid infinite recursion
			_Is_Calling.store(true, std::memory_order::memory_order_release);
			_Lock.lock();
			for(auto i=Slots.begin(); i!= Slots.end(); i++){
				if((*i).first !=0){
					if((*i).first->Is_Calling()) continue;
				}
				auto& cont = (*i).second;
				for(size_t j =0; j < cont.size(); j++){
					cont[j](params...);
				}
			}
			_Lock.unlock();
			_Is_Calling.store(false, std::memory_order::memory_order_release);
		}
		bool Is_Calling() const { return _Is_Calling;}
	protected:
		std::map<Signal_mt<rettype, T...>*, std::vector<std::function<rettype(T...)>>> Slots;
		std::atomic<bool> _Is_Calling;
		std::mutex _Lock;
	};
};


#endif