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

#include "../Functions/LdsFunctions.h"
#include "../Execution/LdsProgram.h"

// List of function arguments
typedef DSList<string> CLdsInlineArgs;

// Global script function
struct LDS_API SLdsFunc {
  int ef_iArgs; // amount of arguments
  LdsFuncPtr ef_pFunc; // pointer to the function

  // Constructors
  SLdsFunc(void) : ef_iArgs(0), ef_pFunc(NULL) {};
  SLdsFunc(int ct, void *pFunc) : ef_iArgs(ct), ef_pFunc((LdsReturn (*)(CLdsValue *))pFunc) {};
};

// Inline function
struct LDS_API SLdsInlineFunc {
  CLdsInlineArgs in_astrArgs; // list of arguments
  CLdsProgram in_pgFunc; // function actions

  CLdsInFuncMap in_mapInlineFunc; // inline functions within this function

  // Constructors
  SLdsInlineFunc(void) {};
  SLdsInlineFunc(const CLdsInlineArgs &astrArgs, const CLdsProgram &pg) {
    in_astrArgs.CopyArray(astrArgs);
    in_pgFunc = pg;
  };

  // Assignment
  SLdsInlineFunc &operator=(const SLdsInlineFunc &inOther) {
    in_astrArgs.CopyArray(inOther.in_astrArgs);
    in_pgFunc = inOther.in_pgFunc;
    in_mapInlineFunc.CopyMap(inOther.in_mapInlineFunc);

    return *this;
  };
};
