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
  string strIndex; // reference index (array index or structure variable)
  bool bIndex; // it's an array index

  // Constructor
  SLdsRefIndex(void);
  // Structure variable constructor
  SLdsRefIndex(const string &strVar);
  // Array index constructor
  SLdsRefIndex(const int &iIndex);

  // Get index as a number
  inline int GetIndex(void) {
    return atoi(strIndex.c_str());
  };
};

// Constructor templates
#define ARGS_TEMP(_Var, _Access, _Name, _Ref, _Const, _Global) \
  vr_pvar(_Var), vr_pvalAccess(_Access), vr_strVar(_Name), vr_strRef(_Ref) \
{ \
  vr_eFlags = ELdsValueRefFlags((_Const  ? VRF_CONST  : 0) \
                              | (_Global ? VRF_GLOBAL : 0)); \
}

#define CONSTRUCTOR_TEMP(_Type) CLdsValueRef(const _Type &_Set) : \
                                             vr_val(_Set), ARGS_TEMP(NULL, NULL, "", "", false, false)

#define CONSTRUCTOR_FULL(_Type) CLdsValueRef(const _Type &_Set, SLdsVar *_Var, CLdsValue *_Access, \
  const string &_Name, const string &_Ref, bool _Const, bool _Global) \
  : vr_val(_Set), ARGS_TEMP(_Var, _Access, _Name, _Ref, _Const, _Global)

// Value reference
class LDS_API CLdsValueRef {
  public:
    CLdsValue vr_val; // value itself
  
    SLdsVar *vr_pvar; // variable reference (from CLdsScriptEngine::_mapLdsVariables or CLdsThread::sth_mapLocals)
    CLdsValue *vr_pvalAccess; // array/structure value reference

    CDList<SLdsRefIndex> vr_ariIndices; // reference indices in order (to determine vr_pvalAccess for I/O)

    string vr_strVar; // variable this value belongs to
    string vr_strRef; // reference name

    enum ELdsValueRefFlags {
      VRF_CONST  = (1 << 0), // reference is a constant variable (for structure variables)
      VRF_GLOBAL = (1 << 1), // referencing a global variable (from CLdsScriptEngine::_mapLdsVariables; for I/O)
    } vr_eFlags;
  
    // Default constructor
    CLdsValueRef(void) : vr_val(0), ARGS_TEMP(NULL, NULL, "", "", false, false);
  
    // Value constructors
    CONSTRUCTOR_TEMP(CLdsValue);
    CONSTRUCTOR_TEMP(int);
    CONSTRUCTOR_TEMP(double);
    CONSTRUCTOR_TEMP(string);
  
    // Full constructors
    CONSTRUCTOR_FULL(CLdsValue);
    CONSTRUCTOR_FULL(int);
    CONSTRUCTOR_FULL(double);
    CONSTRUCTOR_FULL(string);

    // Get value by an array index
    CLdsValue *GetValue(const int &iIndex);
    // Get value by a structure variable name
    CLdsValue *GetValue(const string &strVar);

    // Add array index
    inline void AddIndex(const int &iIndex) {
      vr_ariIndices.Add(SLdsRefIndex(iIndex));
    };

    // Add structure index
    inline void AddIndex(const string &strVar) {
      vr_ariIndices.Add(SLdsRefIndex(strVar));
    };

    // Set the flag
    inline void SetFlag(int iFlag, bool bSet) {
      if (bSet) {
        vr_eFlags = ELdsValueRefFlags(vr_eFlags | iFlag);
      } else {
        vr_eFlags = ELdsValueRefFlags(vr_eFlags & ~iFlag);
      }
    };

    // Check for flags
    inline bool IsConst(void) {
      return (vr_eFlags & VRF_CONST) != 0;
    };
    inline bool IsGlobal(void) {
      return (vr_eFlags & VRF_GLOBAL) != 0;
    };
};

// Undefine constructor templates
#undef CONSTRUCTOR_TEMP
#undef CONSTRUCTOR_FULL
#undef ARGS_TEMP

// A function return type to distinguish the purpose
typedef CLdsValueRef LdsReturn;
