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

// Importing/exporting for Visual Studio
#ifdef _MSC_VER
  #ifdef LDS_EXPORT
    #define LDS_API _declspec(dllexport)
  #else
    #define LDS_API _declspec(dllimport)
  #endif
#endif

#ifdef PLATFORM_UNIX
  #define LDS_API
#endif

// Include the library
#ifndef LDS_EXPORT
  #pragma comment(lib, "LilacDragonScript.lib")
#endif

// Standard library
#include <math.h>
#include <sstream>

// Standard string
#include <string>
typedef std::string string;

// Include Dreamy Structures if needed
#ifndef LDS_EXCLUDE_DSTRUCT
  #include "DreamyStructures/DataStructures.h"
#endif

// Engine pointer
typedef class CLdsScriptEngine *LdsEnginePtr;

// Pre-declare types
class ILdsValueBase;
class CLdsValue;

class CLdsVars;
struct SLdsVar;

struct SLdsFunc;
struct SLdsInlineFunc;

class CLdsToken;
class CBuildNode;
class CCompAction;

struct SLdsCache;

// Special types
typedef unsigned char LdsFlags; // script flags
typedef unsigned long LdsSize; // size in bytes
typedef unsigned long LdsHash; // string hash value

// Lists
typedef DSArray<CLdsValue>            CLdsArray;        // value array
typedef DSList<CLdsValue>             CLdsList;         // value list
typedef DSMap<string, CLdsValue>      CLdsMap;          // value map
typedef DSMap<string, SLdsFunc>       CLdsFuncMap;      // functions map
typedef DSMap<string, SLdsInlineFunc> CLdsInFuncMap;    // inline functions map
typedef DSList<CLdsToken>             CTokenList;       // script token list
typedef DSList<CBuildNode>            CNodeList;        // node list
typedef DSList<CBuildNode *>          CDynamicNodeList; // dynamic node list
typedef DSList<CCompAction>           CActionList;      // action list
typedef DSMap<LdsHash, SLdsCache>     CScriptCache;     // script cache map
typedef DSList<ILdsValueBase *>       CLdsValueTypes;   // value type list

// 64-bit integer
typedef __int64 LONG64;

// I/O function types
typedef void (*CLdsWriteFunc)(void *pStream, const void *pData, const LdsSize &iSize);
typedef void (*CLdsReadFunc)(void *pStream, void *pData, const LdsSize &iSize);

// Printing function type
typedef void (*CLdsPrintFunc)(const char *);

// General
#include "LdsCompatibility.h"
#include "LdsFormatting.h"
#include "LdsCommon.h"
