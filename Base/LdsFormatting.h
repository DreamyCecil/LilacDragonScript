#pragma once

#include "../LdsBase.h"
#include <stdarg.h>

// Format a string
LDS_API string LdsPrintF(const char *strFormat, ...);
// Format some string using a list of arguments
LDS_API string LdsVPrintF(const char *strFormat, va_list arg);

// Convert float number into a string
LDS_API string LdsFloatStr(const float &fNum);

// Print script position
LDS_API string LdsPrintPos(const int &iPos);
