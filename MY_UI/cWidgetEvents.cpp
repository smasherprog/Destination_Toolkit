#include "PCH.h"
#include "cWidgetEvents.h"

void MY_UI::Internal::Caller_Register(cWidgetHandler* pthis, cWidgetCaller* caller){ pthis->Internal_Callers.push_back(caller); }
void MY_UI::Internal::Caller_UnRegister(cWidgetHandler* pthis, cWidgetCaller* caller){
	for(auto i(pthis->Internal_Callers.begin()); i != pthis->Internal_Callers.end(); ){// just in case there are multiple callers to the same object, do a thourough search
		if(caller == *i) {
			i = pthis->Internal_Callers.erase(i);// erase returns the next iterator in the vector
		}
		else ++i;// if an erase did not occur, increment to the next iterator
	}
}
void MY_UI::Internal::Caller_Clear(cWidgetHandler* pthis){
	auto i(pthis->Internal_Callers.begin());
	while(!pthis->Internal_Callers.empty()){
		Caller_UnRegister(pthis, *i);
		(*i)->RemoveHandler(pthis);
		i=pthis->Internal_Callers.begin();
	}
}
void MY_UI::Internal::cWidgetCaller::RemoveHandler(cWidgetHandler* handler){
	Caller_UnRegister(handler, this);// make sure the handler forgets us!!
	// remove all occurances that this class has of the handler
	for(auto i(Handlers.begin()); i != Handlers.end(); ){// just in case there are multiple callers to the same object, do a thourough search
		if(handler == (*i).Object) i = Handlers.erase(i);// erase returns the next iterator in the vector
		else ++i;// if an erase did not occur, increment to the next iterator
	}
}
void MY_UI::Internal::cWidgetCaller::ClearHandlers(){
	for(auto i(Handlers.begin()); i != Handlers.end(); i++){// just in case there are multiple callers to the same object, do a thourough search
		if((*i).Object != 0) Caller_UnRegister((*i).Object, this);
	}
	Handlers.clear();
}
void MY_UI::Internal::cWidgetCaller::Call(Controls::cWidgetBase* thiswidget){
	for(auto i(Handlers.begin()); i != Handlers.end(); i++){
		Handler& handler = *i;
		if ( handler.Function ) handler.Function(thiswidget);
		if ( handler.EmptyFunction ) handler.EmptyFunction();
	}
}
void MY_UI::Internal::cWidgetCaller::Add(std::function<void (Controls::cWidgetBase* widget)> functiontocall, cWidgetHandler* objecttoregister, bool r){// r is never used, but needed because the compiler cannot tell the different between the two Add functions
	Handler h;
	h.Function = functiontocall;
	h.Object = objecttoregister;
	Handlers.push_back( h );
	if(objecttoregister != 0)
		Caller_Register(objecttoregister,  this ); // let the caller know about the connection. This is important !!// Imagine a button that is suppost to call a callback in another part of your program. You pass the objecttocallon pointer, but it gets deleted and the button
	// still persists. If the button tries to call a function on the pointer, an error will occur. So, by registering it, they can let each other know if the other gets deleted
}
void MY_UI::Internal::cWidgetCaller::Add(std::function<void ()> functiontocall, cWidgetHandler* objecttoregister){
	Handler h;
	h.EmptyFunction = functiontocall;
	h.Object = objecttoregister;
	Handlers.push_back( h );
	if(objecttoregister != 0)
		Caller_Register(objecttoregister,  this ); // let the caller know about the connection. This is important !!// Imagine a button that is suppost to call a callback in another part of your program. You pass the objecttocallon pointer, but it gets deleted and the button
}	
void MY_UI::Internal::cWidgetCaller::Add(std::function<void (Controls::cWidgetBase* widget)> functiontocall, bool r){
	Handler h;
	h.Function = functiontocall;
	Handlers.push_back( h );
}
void MY_UI::Internal::cWidgetCaller::Add(std::function<void ()> functiontocall){
	Handler h;
	h.EmptyFunction = functiontocall;
	Handlers.push_back( h );
}
