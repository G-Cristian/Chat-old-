#ifndef _THREADS_DEFINITIONS_H_
#define _THREADS_DEFINITIONS_H_

#include "result.h"
#include <string>

#if defined(WIN32) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#define Mutex HANDLE

#else
//We are not in windows

#error "OS not supported."

#endif

Result mutexWait(Mutex mutex);
Result mutexSignal(Mutex mutex);


#endif