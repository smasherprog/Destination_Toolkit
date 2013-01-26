#ifndef CWIDGETEVENTS_H
#define CWIDGETEVENTS_H
#include <vector>
#include <functional>

typedef void (*Global_Empty_Function)();
namespace MY_UI{
	namespace Controls{
		class cWidgetBase;
	};
};
typedef void (*Global_Function)(MY_UI::Controls::cWidgetBase* object );// this is so you can pass what ever you want to the function and cast it into the appropriate type

namespace MY_UI{
	namespace Controls {
		class cWidgetBase;// forward declaration
	};
	namespace Internal{
		class cWidgetCaller;// forward delcaration
		class cWidgetHandler;// forward delcaration

		
		// functions declared outside cWidgetHandler to keep the interface of the widgets clean
		void Caller_Register(cWidgetHandler* pthis, cWidgetCaller* caller);
		void Caller_UnRegister(cWidgetHandler* pthis, cWidgetCaller* caller);
		void Caller_Clear(cWidgetHandler* pthis);

		class cWidgetHandler{
		public:

			
			virtual ~cWidgetHandler(){ Caller_Clear(this); }
			std::vector<cWidgetCaller*>	Internal_Callers;

		};		
			
		// class to hold functions and the objects to call them on
		class Handler{ 
		public:	

			std::function<void (Controls::cWidgetBase* widget)> Function;
			std::function<void ()> EmptyFunction;

			cWidgetHandler*			Object;

			Handler(): Object(0) { }
		};

		class cWidgetCaller{
		public:
		
			std::vector<Handler> Handlers;
			~cWidgetCaller(){ ClearHandlers(); }
			void ClearHandlers();
			void Call(Controls::cWidgetBase* thiswidget);
			void RemoveHandler(cWidgetHandler* handler);
			

			void Add(std::function<void (Controls::cWidgetBase* widget)> functiontocall, cWidgetHandler* objecttoregister, bool r);
			void Add(std::function<void ()> functiontocall, cWidgetHandler* objecttoregister);
			void Add(std::function<void (Controls::cWidgetBase* widget)> functiontocall, bool r);
			void Add(std::function<void ()> functiontocall);
		};
	};
};

#endif