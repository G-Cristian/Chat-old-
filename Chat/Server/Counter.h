#ifndef _COUNTER_H_
#define _COUNTER_H_

#if defined(WIN32) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#else
//We are not in windows

#error "OS not supported."

#endif

class Counter {
public:
	Counter();
	Counter(unsigned int value);
	~Counter();
	void increment();
	void decrement();
	unsigned int getValue();
	void waitForCero();
private:
	unsigned int m_value;
#if defined(WIN32) || defined(_WIN32)
	CRITICAL_SECTION m_criticalSection;
	CONDITION_VARIABLE m_greaterThanCero;
	CONDITION_VARIABLE m_equalCero;
#else

#error "OS not supported."

#endif
};

#endif
