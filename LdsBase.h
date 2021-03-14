/* Copyright (c) 2021 Dreamy Cecil

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

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
typedef unsigned long LdsSize; // size in bytes

// Lists
typedef CDArray<class CLdsValue> CLdsArray; // value array
typedef CDList<class CLdsValue> CLdsValueList; // value list
typedef CDMap<string, class CLdsValue> CLdsValueMap; // value map
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
