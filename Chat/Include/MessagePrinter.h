#ifndef _MESSAGE_PRINTER_H_
#define _MESSAGE_PRINTER_H_

#include <string>

#if defined(WIN32) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#else
//We are not in windows

#error "OS not supported."

#endif

using namespace std;

#if defined(_DEBUG) || !defined(NDEBUG)
#define ShowMessage(msg) { MessagePrinter *mp = MessagePrinter::getInstance(); mp->showMessage(msg); }
#else
#define ShowMessage(msg) 
#endif

class MessagePrinter {
public:
	static MessagePrinter *getInstance();
	static void dispose();
	void showMessage(string message);
private:
	MessagePrinter();
	~MessagePrinter();

	static MessagePrinter *m_instance;

#if defined(WIN32) || defined(_WIN32)
	CRITICAL_SECTION m_criticalSection;
#else

#error "OS not supported."

#endif
};

#endif
