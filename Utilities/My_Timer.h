#ifndef CMYTIME_H
#define CMYTIME_H
#include "stdint.h"
#include "limits.h"
#if _WIN32||_WIN64
#include "windows.h"
#endif

namespace My_Timer {

	class tick_count {
	public:
		//! Relative time interval.
		class interval_t {
		public:
			long long value;
			explicit interval_t( long long value_ ) : value(value_) {}
			//! Construct a time interval representing zero time duration
			interval_t() : value(0) {};
			//! Construct a time interval representing sec seconds time  duration
			explicit interval_t( double sec ){
				LARGE_INTEGER qpfreq;
				QueryPerformanceFrequency(&qpfreq);
				value = static_cast<long long>(sec*qpfreq.QuadPart);
			}

			double tick_count::interval_t::seconds() const {
				LARGE_INTEGER qpfreq;
				QueryPerformanceFrequency(&qpfreq);
				return value/(double)qpfreq.QuadPart;
			}
			double tick_count::interval_t::micro_seconds() const {
				LARGE_INTEGER qpfreq;
				QueryPerformanceFrequency(&qpfreq);
				return (value/(double)qpfreq.QuadPart)*1000.0;
			}
			double tick_count::interval_t::nano_seconds() const {
				LARGE_INTEGER qpfreq;
				QueryPerformanceFrequency(&qpfreq);
				return (value/(double)qpfreq.QuadPart)*1000000000.0;
			}

			//! Subtract two intervals.
			friend interval_t operator-( const interval_t& i, const interval_t& j ) {
				return interval_t(i.value-j.value);
			}

			//! Accumulation operator
			interval_t& operator+=( const interval_t& i ) {value += i.value; return *this;}

			//! Subtraction operator
			interval_t& operator-=( const interval_t& i ) {value -= i.value; return *this;}
			friend class tick_count;
		};

		//! Construct an absolute timestamp initialized to zero.
		tick_count() : my_count(0) {};

		//! Return current time.
		static tick_count tick_count::now() {
			tick_count result;
			LARGE_INTEGER qpcnt;
			QueryPerformanceCounter(&qpcnt);
			result.my_count = qpcnt.QuadPart;
			return result;
		}
		long long my_count;
	};


	//! Extract the intervals from the tick_counts and subtract them.
	inline My_Timer::tick_count::interval_t operator-( const My_Timer::tick_count& t1, const My_Timer::tick_count& t0 ){
		return My_Timer::tick_count::interval_t( t1.my_count-t0.my_count );
	}

	//! Add two intervals.
	inline My_Timer::tick_count::interval_t operator+( const My_Timer::tick_count::interval_t& i, const My_Timer::tick_count::interval_t& j ) {
		return My_Timer::tick_count::interval_t(i.value+j.value);
	}
	class Frame_Timer{
	public:

		Frame_Timer(){ Reset(); }
		void Reset(){
			DT=TotalDT=0.0;
			lTime=LastSecond=tick_count::now();
			FPS=Frames=0;
		}
		void Per_Loop(){	
			tick_count g_Time=tick_count::now();
			Frames++;
			if((g_Time - LastSecond).seconds() >=1.0){
				FPS = Frames;
				Frames =0;
				LastSecond=g_Time;
			}
			DT=(g_Time-lTime).seconds();
			lTime = g_Time;
			TotalDT+=DT;
		}

		double DT, TotalDT;
		tick_count lTime, LastSecond;
		size_t FPS, Frames;
	};

};


#endif
