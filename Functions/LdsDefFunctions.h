#pragma once

#include "../LdsScriptEngine.h"

// Debug output
LdsReturn LdsDebugOut(SLdsValue *pArgs);

// Print a hexadecimal number
LdsReturn LdsPrintHex(SLdsValue *pArgs);

// Pause the script execution
LdsReturn LdsWait(SLdsValue *pArgs);
