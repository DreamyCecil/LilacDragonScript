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

#include "../Values/LdsValue.h"

// Value reference index
struct LDS_API SLdsRefIndex {
  string strIndex; // reference index (array index or object property)
  bool bIndex; // it's an array index

  // Default constructor
  SLdsRefIndex(void);

  // Property constructor
  SLdsRefIndex(const string &strVar);

  // Index constructor
  SLdsRefIndex(const int &iIndex);

  // Get index as a number
  inline int GetIndex(void) {
    return atoi(strIndex.c_str());
  };
};

// Value reference
class LDS_API CLdsValueRef {
  public:
    CLdsValue vr_val; // value itself

    SLdsVar *vr_pvar; // variable reference (from CLdsScriptEngine::_mapLdsVariables or CLdsThread::sth_mapLocals)
    SLdsVar *vr_pvarAccess; // array/object property reference

    DSList<SLdsRefIndex> vr_ariIndices; // reference indices in order (to determine vr_pvalAccess for I/O)

    enum ELdsValueRefFlags {
      VRF_GLOBAL = (1 << 0), // referencing a global variable (from CLdsScriptEngine::_mapLdsVariables; for I/O)
    };

    LdsFlags vr_ubFlags;

    // Constructors
    CLdsValueRef(void);
    CLdsValueRef(const CLdsValue &val);
    CLdsValueRef(const CLdsValue &val, SLdsVar *pvar, SLdsVar *pvarAccess, const LdsFlags &ubFlags);

    // Assignment
    CLdsValueRef &operator=(const CLdsValueRef &vrOther);

    // Get variable by index
    SLdsVar *AccessVariable(const int &iIndex);
    // Get variable by name
    SLdsVar *AccessVariable(const string &strVar);

    // Add index
    inline void AddIndex(const int &iIndex) {
      vr_ariIndices.Add() = SLdsRefIndex(iIndex);
    };

    // Add property
    inline void AddIndex(const string &strVar) {
      vr_ariIndices.Add() = SLdsRefIndex(strVar);
    };

    // Set the flag
    inline void SetFlag(const LdsFlags &iFlag, const bool &bSet) {
      if (bSet) {
        vr_ubFlags |= iFlag;
      } else {
        vr_ubFlags &= ~iFlag;
      }
    };

    // Get flags
    inline LdsFlags &GetFlags(void) {
      return vr_ubFlags;
    };

    // Check for flags
    inline LdsFlags IsGlobal(void) {
      return (vr_ubFlags & VRF_GLOBAL);
    };
};

// A function return type to distinguish the purpose
typedef CLdsValueRef LdsReturn;
