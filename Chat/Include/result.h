#ifndef _RESULT_H_
#define _RESULT_H_

#include <string>

using namespace std;

enum ResultType{ R_OK = 1, R_ERROR = 2, R_WARNING = 3 };

struct Result
{
	ResultType type;
	string message;
};

#endif