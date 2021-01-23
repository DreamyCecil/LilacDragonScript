#pragma once

#include "../LdsScriptEngine.h"

// Debug output
LdsReturn LdsDebugOut(CLdsValue *pArgs);

// Print a hexadecimal number
LdsReturn LdsPrintHex(CLdsValue *pArgs);

// Pause the script execution
LdsReturn LdsWait(CLdsValue *pArgs);
