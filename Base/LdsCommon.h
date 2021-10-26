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

#include "../LdsBase.h"

// LDS error codes
enum ELdsError {
  // Generic
  LER_OK = 0x00, // no error

  LER_VALUE = 0x01, // value error
  LER_WRITE = 0x02, // cannot write data
  LER_READ  = 0x03, // cannot read data

  // Parser errors
  LEP_STRING  = 0x10, // unclosed string
  LEP_DIR     = 0x11, // unknown directive
  LEP_DIRNAME = 0x12, // expected directive name
  LEP_CHAR    = 0x13, // unexpected character

  // Builder errors
  LEB_STOPPED   = 0x20, // couldn't build all tokens
  LEB_EMPTY     = 0x21, // nothing to build
  LEB_TOKEN     = 0x22, // unexpected token
  LEB_STATEMENT = 0x23, // expected a statement

  LEB_ID        = 0x24, // expected an identifier
  LEB_VALUE     = 0x25, // expected a value
  LEB_OPERATOR  = 0x26, // unexpected operator type

  LEB_OPENCB    = 0x27, // expected opening curly bracket
  LEB_CLOSECB   = 0x28, // unclosed pair of curly brackets
  LEB_OPENP     = 0x29, // expected opening parenthesis
  LEB_CLOSEP    = 0x2a, // unclosed pair of parentheses
  LEB_CLOSESB   = 0x2b, // unclosed pair of square brackets

  LEB_COLON     = 0x2c, // expected a colon
  LEB_COMMA     = 0x2d, // expected a comma
  LEB_ASSIGN    = 0x2e, // expected assignment sign

  LEB_DEFAULT   = 0x2f, // too many default cases
  LEB_SWITCH    = 0x30, // expected next switch case or switch block end
  LEB_WHILE     = 0x31, // expected 'while' after 'do'
  LEB_ARRAY     = 0x32, // expected next array value or array end
  LEB_OBJECT    = 0x33, // expected next object property or object end
  LEB_FUNCTION  = 0x34, // expected next function argument or function end
  LEB_FUNCDEF   = 0x35, // function redefinition

  LEB_BREAK     = 0x36, // cannot break
  LEB_CONTINUE  = 0x37, // cannot continue

  // Compiler errors
  LEC_NODE       = 0x40, // unknown build node
  LEC_NOVAR      = 0x41, // variable doesn't exist
  LEC_NOFUNC     = 0x42, // function doesn't exist
  LEC_VARDEF     = 0x43, // variable redefinition
  LEC_CONST      = 0x44, // cannot change constant variable
  LEC_INVALIDEXP = 0x45, // cannot compile the expression
  LEC_FUNCARG    = 0x46, // function argument count mismatch

  // Execution errors
  LEX_EMPTY     = 0x50, // no actions to execute
  LEX_ACTION    = 0x51, // cannot run action
  LEX_PAUSE     = 0x52, // no thread to pause
  LEX_QUICKRUN  = 0x53, // cannot pause the thread in quick run mode
  LEX_THREAD    = 0x54, // thread failure
  LEX_EVALUATOR = 0x55, // evaluator failure

  LEX_VARIABLE   = 0x60, // invalid variable
  LEX_CONST      = 0x61, // cannot change constant variable
  LEX_UNARY      = 0x62, // cannot perform unary operation
  LEX_BINARY     = 0x63, // cannot perform binary operation
  LEX_ARRAYEMPTY = 0x64, // cannot index an empty array
  LEX_ARRAYOUT   = 0x65, // array index out of bounds
  LEX_OBJECTACC  = 0x66, // wrong object accessor
  LEX_EXPECTACC  = 0x67, // expected object member
  LEX_OBJECTMEM  = 0x68, // invalid object member
  LEX_ACCESS     = 0x69, // wrong accessor type
  LEX_NOACCESS   = 0x6a, // no accessor reference
  LEX_NOFUNC     = 0x6b, // no function pointer
  LEX_CALL       = 0x6c, // external function error
};

// LDS error
struct LDS_API SLdsError {
  ELdsError le_eError;
  string le_strMessage;

  // Constructor
  SLdsError(const ELdsError &eError, const string &strMessage) :
    le_eError(eError), le_strMessage(strMessage) {};
};

// Throw formatted exception
LDS_API void LdsThrow(const ELdsError &eError, const char *strFormat, ...);

// Calculate simple hash value out of some string
LDS_API LdsHash GetHash(const string &str);

// General reporting function
LDS_API extern CLdsPrintFunc LDS_pLogFunction;
LDS_API void LdsLog(const char *strFormat, ...);
