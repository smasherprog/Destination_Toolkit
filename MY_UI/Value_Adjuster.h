#ifndef VALUE_ADJUSTER_H
#define VALUE_ADJUSTER_H
#include "cWidgetBase.h"

namespace MY_UI{

	namespace Controls{
		class Value_Adjuster: public cWidgetBase{
		public:

			Value_Adjuster(cWidgetBase* parent);
			~Value_Adjuster() {}

			virtual void LeftMouseButtonUp();
		
			virtual void SetFocus(bool focus);
			virtual void Draw_Clip();
			virtual void Draw_NoClip();
			virtual void Close();
			virtual void Update();
			virtual void SetMaxCallsPerSecond(float c){ MaxChangesPerSecond=c;  }// this is the max number of times per second that either increase or decrease will be called per second
			virtual float GetMaxCallsPerSecond(){ return MaxChangesPerSecond; }

			float MaxChangesPerSecond;
			unsigned int LastMovementCall;// a timer so I know when I called either OnIncrease or OnDecrease last
			MY_UI::Internal::cWidgetCaller OnIncrease, OnDecrease;

		};
	};
};

#endif