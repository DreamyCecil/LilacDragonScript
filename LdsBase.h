#pragma once

// Standard string
#include <string>
typedef std::string string;

// For importing
#ifndef LDS_API
#define LDS_API _declspec(dllexport)
#endif

// Include structures if needed
#ifndef LDS_EXCLUDE_DSTRUCT
#include "DreamyStructures/DataStructures.h"
#endif

#include "Base/LdsCompatibility.h"
#include "Base/LdsFormatting.h"

// Special types
typedef unsigned char LdsFlags; // Script flags

// Lists
typedef CDList<struct SLdsValue> CLdsValueList; // Value list
typedef CDMap<string, struct SLdsFunc> CLdsFuncMap; // Functions map
typedef CDMap<string, struct SLdsInlineFunc> CLdsInFuncMap; // Inline functions map
typedef CDMap<string, struct SLdsVar> CLdsVarMap; // Variables map
typedef CDList<class CLdsToken> CTokenList; // Script token list
typedef CDList<class CBuildNode> CNodeList; // Node list
typedef CDList<class CBuildNode *> CDynamicNodeList; // Dynamic node list
typedef CDList<class CCompAction> CActionList; // Action list

// 64-bit integer
typedef __int64 LONG64;
