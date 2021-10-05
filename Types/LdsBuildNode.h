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

// Build node type
enum EBuildNode {
  // values
  EBN_RAW_VAL,    // simple pure value
  EBN_ARRAY_VAL,  // array of values (arg: array size)
  EBN_OBJECT_VAL, // object with property fields (arg: amount of properties)
  EBN_IDENTIFIER, // identifier

  // operations
  EBN_UNARY_OP,  // unary operator
  EBN_BINARY_OP, // binary operator
  EBN_ASSIGN_OP, // assignment operator
  
  EBN_PREFIX,  // ++i
  EBN_POSTFIX, // i++
  EBN_ADJFIX,  // i += 1
  EBN_ACCESS,  // variable accessor (arg: array or object)
  
  // actions
  EBN_CALL_ACT,   // function call (arg: amount of arguments)
  EBN_RETURN_ACT, // return from the script (arg: has return value)
  EBN_DISCARD_ACT,
  EBN_BREAK_ACT,
  EBN_CONTINUE_ACT,

  // definitions
  EBN_FUNC_DEF, // inline function definition
  EBN_VAR_DEF,  // variable definition (arg: constant)
  EBN_SVAR_DEF, // object property definition (arg: constant)
  
  // blocks
  EBN_BLOCK, // (arg: amount of expressions)
  EBN_IF_THEN,
  EBN_IF_THEN_ELSE,
  EBN_SWITCH, // (arg: amount of options)
  
  // loops
  EBN_WHILE_LOOP,
  EBN_DO_LOOP,
  EBN_FOR_LOOP,
  
  EBN_DIR, // thread directive (arg: directive type)

  EBN_LAST,
};

// Build node names
static const char *_astrBuildNodes[EBN_LAST] = {
  "RAW_VAL", "ARRAY_VAL", "OBJECT_VAL", "IDENTIFIER",
  "UNARY_OP", "BINARY_OP", "ASSIGN_OP",
  "PREFIX", "POSTFIX", "ADJFIX", "ACCESS",
  "CALL_ACT", "RETURN_ACT", "DISCARD_ACT",
  "BREAK_ACT", "CONTINUE_ACT",
  "FUNC_DEF", "VAR_DEF", "SVAR_DEF",
  "BLOCK", "IF_THEN", "IF_THEN_ELSE", "SWITCH",
  "WHILE_LOOP", "DO_LOOP", "FOR_LOOP",
  "DIR",
};

// Build node argument names
static const char *_astrBuildNodeArguments[EBN_LAST] = {
  "", "Values", "Vars", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "Const", "Const", "Acts", "",
  "", "Opts", "", "", "", "",
};

// Thread directives
enum EThreadDir {
  THD_DEBUGCONTEXT, // debug output with a context level
};

// Expression build flags
#define LBF_NONE  (0) // no flags
#define LBF_NOOPS (1 << 0) // don't apply operations to the expression
#define LBF_SCOPE (1 << 1) // build only identifiers

// Build Node
class LDS_API CBuildNode : public CLdsToken {
  public:
    // Optional node references
    CDynamicNodeList bn_abnNodes;
    
    // Default constructor
    CBuildNode(void) : CLdsToken() {};

    // Value constructor
    CBuildNode(const EBuildNode &eType, const int &iPos, const CLdsValue &valValue, const int &iArg, CNodeList *abnNodes = NULL) :
      CLdsToken(eType, iPos, valValue, iArg)
    {
      if (abnNodes != NULL) {
        CopyNodes(*abnNodes);
      }
    };
    
    // Copy constructor
    CBuildNode(const CBuildNode &bnOther) : CLdsToken() {
      operator=(bnOther);
    };
    
    // Destructor
    ~CBuildNode(void) {
      RemoveReferences(-1);
    };

    // Copy from another node
    CBuildNode &operator=(const CBuildNode &bnOther);

    // Add new node reference
    void AddReference(CBuildNode *pbn);
    // Remove node references
    void RemoveReferences(int iPos);

    // Copy nodes from some dynamic list
    int CopyNodes(const CDynamicNodeList &abnNodes);
    // Copy nodes from some list
    int CopyNodes(CNodeList &abnNodes);

    // Print out the node
    string Print(string strTab);
};
