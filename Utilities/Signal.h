#ifndef MY_SIGNAL_H
#define MY_SIGNAL_H
#include <vector>
#include <map>
#include <functional>


namespace MY_Utilities{

	template <typename rettype, typename... T> class Signal{
	public:
		Signal(){}
		~Signal(){ for(auto i=Slots.begin(); i != Slots.end(); i++) (*i).first->Disconnect(this); }

		void Connect(Signal<rettype, T...>* othersig){  Slots[othersig] = std::vector<std::function<rettype(T...)>>(); } 
		void Connect(Signal<rettype, T...>* othersig, std::function<rettype(T...)> f){  
			auto i = Slots.find(othersig);
			if(i!=Slots.end()) (*i).second.push_back(f);
			else Slots[othersig].push_back(f);
		}
		void Disconnect(Signal<rettype, T...>* othersig){ Slots.erase(othersig); }

		void Call(T... params){
			for(auto i=Slots.begin(); i!= Slots.end(); i++){
				auto& cont = (*i).second;
				for(size_t j =0; j < cont.size(); j++){
					cont[j](params...);
				}
			}
		}

	protected:
		std::map<Signal<rettype, T...>*, std::vector<std::function<rettype(T...)>>> Slots;
	};

};


#endif