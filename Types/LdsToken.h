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

#include "Values/LdsValue.h"

// TODO: Add '::' semi-operator that will access specific scope for a variable/function.
//       E.g. if inside the inline function and need to access outside variable/function that was redefined, should use '::Out(::strName)'.
//       Same with scopes of inline function above. If inside the inline function twice, can also use 'PrevFunc::MyFunc(FirstFunc::var)'.
//       Or just use a dot operator and make objects kinda like definitive values with custom functions and variables but no statements.
// Script token types
enum ELdsToken {
  LTK_END, // end of the script

  LTK_OPERATOR, // binary/unary operator
  LTK_UNARYOP,  // unary operator
  LTK_SET,      // assignment
  LTK_ADJFIX,   // ++/--
  LTK_ACCESS,   // array/object accessor
  
  LTK_PAR_OPEN,  // opening parenthesis
  LTK_PAR_CLOSE, // closing parenthesis
  LTK_CUR_OPEN,  // opening curly bracket
  LTK_CUR_CLOSE, // closing curly bracket
  LTK_SQ_OPEN,   // opening square bracket
  LTK_SQ_CLOSE,  // closing square bracket
  
  LTK_VAL,    // value
  LTK_VAR,    // variable
  LTK_ID,     // identifier
  LTK_STATIC, // static object
  
  LTK_COMMA,   // comma
  LTK_SEMICOL, // statement end
  LTK_COLON,   // special expression end
  LTK_HASH,    // hash symbol
  
  LTK_IF,   // if
  LTK_ELSE, // else
  
  LTK_WHILE, // while-loop
  LTK_DO,    // do-loop
  LTK_FOR,   // for-loop
  
  LTK_SWITCH,  // switch block
  LTK_CASE,    // switch option
  LTK_DEFAULT, // switch default option
  
  LTK_BREAK,    // exit the loop
  LTK_CONTINUE, // skip through the loop
  LTK_RETURN,   // return
  
  LTK_FUNC, // function definition
};

// Script operators in order of priority
enum ELdsOperator {
  LOP_SET = 0x00, // = assignment
  
  LOP_MUL  = 0x01, // * multiplication
  LOP_DIV  = 0x02, // / division
  LOP_FMOD = 0x03, // % modulo
  LOP_IDIV = 0x04, // integer division
  
  LOP_ADD  = 0x10, // + add
  LOP_SUB  = 0x11, // - subtract
  
  LOP_SH_L  = 0x20, // << shift to the left
  LOP_SH_R  = 0x21, // >> shift to the right
  LOP_B_AND = 0x30, // &  bitwise and
  LOP_B_XOR = 0x31, // ^  bitwise xor
  LOP_B_OR  = 0x32, // |  bitwise or
  
  LOP_EQ  = 0x40, // == equal
  LOP_NEQ = 0x41, // != not equal
  LOP_LT  = 0x42, // <  less than
  LOP_LOE = 0x43, // <= less or equal
  LOP_GT  = 0x44, // >  greater than
  LOP_GOE = 0x45, // >= greater or equal
  
  LOP_AND = 0x50, // &&
  LOP_XOR = 0x51, // ^^
  LOP_OR  = 0x52, // ||
  
  LOP_MAX = 0x60, // maximum priority
  
  LOP_ACCESS = 0x61, // array/object accessor

  LOP_COUNT,
};

// Binary operation symbols
static const struct SLdsBinaryOps {
  DSArray<string> astrOperations;

  // Constructor
  SLdsBinaryOps(void) {
    astrOperations.New(LOP_COUNT);

    astrOperations[LOP_SET   ] = "=";
    astrOperations[LOP_MUL   ] = "*";
    astrOperations[LOP_DIV   ] = "/";
    astrOperations[LOP_FMOD  ] = "%";
    astrOperations[LOP_IDIV  ] = "i";
    astrOperations[LOP_ADD   ] = "+";
    astrOperations[LOP_SUB   ] = "-";
    astrOperations[LOP_SH_L  ] = "<<";
    astrOperations[LOP_SH_R  ] = ">>";
    astrOperations[LOP_B_AND ] = "&";
    astrOperations[LOP_B_XOR ] = "^";
    astrOperations[LOP_B_OR  ] = "|";
    astrOperations[LOP_EQ    ] = "==";
    astrOperations[LOP_NEQ   ] = "!=";
    astrOperations[LOP_LT    ] = "<";
    astrOperations[LOP_LOE   ] = "<=";
    astrOperations[LOP_GT    ] = ">";
    astrOperations[LOP_GOE   ] = ">=";
    astrOperations[LOP_AND   ] = "&&";
    astrOperations[LOP_XOR   ] = "^^";
    astrOperations[LOP_OR    ] = "||";
    astrOperations[LOP_MAX   ] = "<max>";
    astrOperations[LOP_ACCESS] = "[]";
  };

  // Get operation name
  const char *operator[](const int &iOperation) const {
    return astrOperations[iOperation].c_str();
  };
} _astrBinaryOps;

// Unary operations
static const string _strUnaryNegate    = "-";
static const string _strUnaryInvert    = "!";
static const string _strUnaryBInvert   = "~";
static const string _strUnaryStringify = "$";

// Script token template
class LDS_API CLdsToken {
  public:
    int lt_eType; // token type
    int lt_iPos; // token place
    int lt_iArg; // token argument

    CLdsValue lt_valValue;

  public:
    // Constructors
    CLdsToken(void) :
      lt_eType(LTK_END), lt_iPos(0), lt_valValue(), lt_iArg(-1) {};

    CLdsToken(const int &iType, const int &iLine, const int &iArg) :
      lt_eType(iType), lt_iPos(iLine), lt_valValue(), lt_iArg(iArg) {};

    CLdsToken(const int &iType, const int &iLine, const CLdsValue &val, const int &iArg) :
      lt_eType(iType), lt_iPos(iLine), lt_valValue(val), lt_iArg(iArg) {};

    // Destructor
    virtual ~CLdsToken(void) {};

    // Assignment
    CLdsToken &operator=(const CLdsToken &ltOther) {
      lt_eType = ltOther.lt_eType;
      lt_iPos = ltOther.lt_iPos;
      lt_iArg = ltOther.lt_iArg;
      lt_valValue = ltOther.lt_valValue;

      return *this;
    };

    // Token position
    string PrintPos(void) const {
      return LdsPrintPos(lt_iPos);
    };

    // Quick value access
    inline ILdsValueBase *operator->(void) const {
      return lt_valValue.val_pBase;
    };
    inline operator ILdsValueBase*(void) const {
      return lt_valValue.val_pBase;
    };
    inline ILdsValueBase &operator*(void) const {
      return *lt_valValue.val_pBase;
    };
};
