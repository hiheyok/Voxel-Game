#ifndef CLOCK_H_
#define CLOCK_H_
#define _CRTDBG_MAP_ALLOC
void precisePause(double nanoseconds);
void timerSleep(double seconds);
void timerSleepNotPrecise(int milliseconds);
//class PreciseTimer {
//public:
//	void set();
//	double getTime();
//private:
//	auto time0;
//}
//


#endif