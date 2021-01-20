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

// Special types
typedef unsigned char LdsFlags; // script flags

// Lists
typedef CDList<struct SLdsValue> CLdsValueList; // value list
typedef CDMap<string, struct SLdsFunc> CLdsFuncMap; // functions map
typedef CDMap<string, struct SLdsInlineFunc> CLdsInFuncMap; // inline functions map
typedef CDMap<string, struct SLdsVar> CLdsVarMap; // variables map
typedef CDList<class CLdsToken> CTokenList; // script token list
typedef CDList<class CBuildNode> CNodeList; // node list
typedef CDList<class CBuildNode *> CDynamicNodeList; // dynamic node list
typedef CDList<class CCompAction> CActionList; // action list

// 64-bit integer
typedef __int64 LONG64;

// General
#include "Base/LdsCompatibility.h"
#include "Base/LdsFormatting.h"
