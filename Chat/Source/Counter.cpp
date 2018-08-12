#include "Counter.h"
#include "MessagePrinter.h"

#include <sstream>

#if defined(WIN32) || defined(_WIN32)

Counter::Counter() {
	Counter::Counter(0);
}

Counter::Counter(unsigned int value) {
	m_value = value;
	InitializeCriticalSection(&m_criticalSection);
	InitializeConditionVariable(&m_greaterThanCero);
	InitializeConditionVariable(&m_equalCero);
}

Counter::~Counter() {
	DeleteCriticalSection(&m_criticalSection);
}

void Counter::increment() {
	EnterCriticalSection(&m_criticalSection);

	m_value++;

	LeaveCriticalSection(&m_criticalSection);

	WakeConditionVariable(&m_greaterThanCero);
}

void Counter::decrement() {
	EnterCriticalSection(&m_criticalSection);

	bool isCero = false;

	while (m_value == 0) {
		
		if (SleepConditionVariableCS(&m_greaterThanCero, &m_criticalSection, INFINITE) == 0) {
			DWORD errorCode = GetLastError();
			std::ostringstream message;
			message << "Error in 'SleepConditionVariableCS' in 'Counter::decrement()'\n";
			message << "|______ Error Code: " << errorCode << "\n";
			ShowMessage(message.str());
		}
	}

	m_value--;

	isCero = m_value == 0;

	LeaveCriticalSection(&m_criticalSection);

	if (isCero) {
		WakeConditionVariable(&m_equalCero);
	}
}

unsigned int Counter::getValue() {
	EnterCriticalSection(&m_criticalSection);
	unsigned int value = m_value;
	LeaveCriticalSection(&m_criticalSection);

	return value;
}

void Counter::waitForCero() {
	EnterCriticalSection(&m_criticalSection);

	while (m_value != 0) {
		if (SleepConditionVariableCS(&m_equalCero, &m_criticalSection, INFINITE) == 0) {
			DWORD errorCode = GetLastError();
			std::ostringstream message;
			message << "Error in 'SleepConditionVariableCS' in 'Counter::waitForCero()'\n";
			message << "|______ Error Code: " << errorCode << "\n";
			ShowMessage(message.str());
		}
	}

	LeaveCriticalSection(&m_criticalSection);
}

#else

#error "OS not supported."

#endif