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

#include "LdsToken.h"
#include "LdsFunc.h"

// Compiler action type
enum ELdsAction {
  LCA_UNKNOWN,
  
  LCA_VAL, // value
  LCA_UN,  // unary operation
  LCA_BIN, // binary operation
  
  LCA_CALL,   // function call
  LCA_INLINE, // inline function call
  LCA_FUNC,   // inline function definition
  LCA_VAR,    // variable definition
  
  LCA_SET, // set value
  LCA_GET, // get value
  LCA_SET_ACCESS, // set value through the accessor
  
  LCA_JUMP, // jump to another action
  LCA_JUMPIF, // jump if true
  LCA_JUMPUNLESS, // jump if false
  LCA_AND,
  LCA_OR,
  LCA_SWITCH, // switch block
  
  LCA_RETURN, // finish execution
  LCA_DISCARD, // discard the last entry
  LCA_DUP, // duplicate the last entry
  
  LCA_DIR, // thread directive
  
  LCA_SIZEOF,
};

// Action names
extern const char *_astrActionNames[LCA_SIZEOF];

// Compiler action
class LDS_API CCompAction : public CLdsToken {
  public:
    SLdsInlineFunc ca_inFunc; // inline function
    
    // Default constructor
    CCompAction(void) : CLdsToken() {};
    
    // Constructors
    CCompAction(const int &iType, const int &iLine, const int &iArg) :
      CLdsToken(iType, iLine, iArg) {};
      
    CCompAction(const int &iType, const int &iLine, const CLdsValue &val, const int &iArg) :
      CLdsToken(iType, iLine, val, iArg) {};
};
