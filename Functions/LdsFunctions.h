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

#include "../Base/LdsScriptEngine.h"

// LDS function arguments
#define LDS_ARGS CLdsValue *_LDS_FuncArgs

// Get value of the next function argument
#define LDS_NEXT_ARG (*_LDS_FuncArgs++)
#define LDS_NEXT_LIST(_MinVars) (LDS_NEXT_ARG.AssertList(_MinVars))

#define LDS_NEXT_INT (LDS_NEXT_ARG.Assert(CLdsIntType())->GetIndex())
#define LDS_NEXT_NUM (LDS_NEXT_ARG.Assert(CLdsFloatType())->GetNumber())
#define LDS_NEXT_STR (LDS_NEXT_ARG.Assert(CLdsStringType())->GetString())
#define LDS_NEXT_ARR (LDS_NEXT_ARG.Assert(CLdsArrayType()))
#define LDS_NEXT_OBJ (LDS_NEXT_ARG.Assert(CLdsObjectType()))

// Script function template
#define LDS_FUNC(_Name) LdsReturn _Name(LDS_ARGS)

// Script function pointer
typedef LdsReturn (*LdsFuncPtr)(LDS_ARGS);

// Map of function pointers
typedef DSMap<string, LdsFuncPtr> CLdsFuncPtrMap;

// External function error
LDS_API void LdsError(const char *strFormat, ...);
