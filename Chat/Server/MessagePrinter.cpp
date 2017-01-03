#include "MessagePrinter.h"
#include <iostream>

using namespace std;

MessagePrinter* MessagePrinter::m_instance = NULL;

MessagePrinter::MessagePrinter() {
	InitializeCriticalSection(&m_criticalSection);
}

MessagePrinter::~MessagePrinter() {
	DeleteCriticalSection(&m_criticalSection);
}

MessagePrinter*  MessagePrinter::getInstance() {
	if (m_instance == NULL)
		m_instance = new MessagePrinter();

	return m_instance;
}

void MessagePrinter::dispose() {
	if (m_instance != NULL) {
		delete m_instance;
		m_instance = NULL;
	}
}

void MessagePrinter::showMessage(string message) {
	EnterCriticalSection(&m_criticalSection);

	cout << message << "\n";

	LeaveCriticalSection(&m_criticalSection);
}