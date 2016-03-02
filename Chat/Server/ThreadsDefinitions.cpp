#include "ThreadsDefinitions.h"

Result mutexWait(Mutex mutex){
	ResultType resultType;
	std::string message;

#if defined(WIN32) || defined(_WIN32)
	DWORD waitResult = WaitForSingleObject(mutex, INFINITE);
	switch (waitResult)
	{
	case WAIT_OBJECT_0:
		resultType = R_OK;
		message = "Wait succeeded.";
		break;
	case WAIT_ABANDONED:
		resultType = R_ERROR;
		message = "Mutex abandoned.";
		break;
	case WAIT_FAILED:
		resultType = R_ERROR;
		message = "Wait failed.";
		break;
	}
#else
	//We are not in windows

#error "OS not supported."

#endif
	return{ resultType, message };
}

Result mutexSignal(Mutex mutex){
	ResultType resultType;
	std::string message;

#if defined(WIN32) || defined(_WIN32)

	BOOL releaseResult = ReleaseMutex(mutex);
	if (releaseResult)
	{
		resultType = R_OK;
		message = "Release succeeded.";
	}
	else
	{
		resultType = R_OK;
		message = "Couldn't release mutex.";
	}

#else
	//We are not in windows

#error "OS not supported."

#endif

	return{ resultType, message };
}
