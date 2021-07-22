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
  EBN_STRUCT_VAL, // structure with value fields (arg: amount of variables)
  EBN_IDENTIFIER, // identifier

  // operations
  EBN_UNARY_OP,  // unary operator
  EBN_BINARY_OP, // binary operator
  EBN_ASSIGN_OP, // assignment operator
  
  EBN_PREFIX,  // ++i
  EBN_POSTFIX, // i++
  EBN_ADJFIX,  // i += 1
  EBN_ACCESS,  // variable accessor (arg: array or struct accessor)
  
  // actions
  EBN_CALL_ACT,   // function call (arg: amount of arguments)
  EBN_RETURN_ACT, // return from the script (arg: has return value)
  EBN_DISCARD_ACT,
  EBN_BREAK_ACT,
  EBN_CONTINUE_ACT,

  // definitions
  EBN_FUNC_DEF, // inline function definition
  EBN_VAR_DEF,  // variable definition (arg: constant)
  EBN_SVAR_DEF, // structure variable definition (arg: constant)
  
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
  "RAW_VAL", "ARRAY_VAL", "STRUCT_VAL", "IDENTIFIER",
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
    CBuildNode &operator=(const CBuildNode &bnOther) {
      if (this == &bnOther) {
        return *this;
      }
      
      // delete old pointers
      RemoveReferences(-1);

      // copy parameters
      lt_eType = bnOther.lt_eType;
      lt_iPos = bnOther.lt_iPos;
      lt_iArg = bnOther.lt_iArg;
      lt_valValue = bnOther.lt_valValue;

      // copy referenced nodes
      CopyNodes(bnOther.bn_abnNodes);

      return *this;
    };

    // Add new node reference
    void AddReference(CBuildNode *pbn) {
      if (pbn != NULL) {
        bn_abnNodes.Add(new CBuildNode(*pbn));
      }
    };

    // Remove node references
    void RemoveReferences(int iPos) {
      // remove one node
      if (iPos != -1) {
        delete bn_abnNodes[iPos];
        bn_abnNodes.Delete(iPos);
        return;
      }

      // remove all nodes
      int ctNodes = bn_abnNodes.Count();

      while (ctNodes-- > 0) {
        delete bn_abnNodes[0];
        bn_abnNodes.Delete(0);
      }
    };

    // Copy nodes from some dynamic list
    int CopyNodes(const CDynamicNodeList &abnNodes) {
      int ctNodes = abnNodes.Count();

      for (int iNode = 0; iNode < ctNodes; iNode++) {
        AddReference(abnNodes[iNode]);
      }

      return ctNodes;
    };
    
    // Copy nodes from some list
    int CopyNodes(CNodeList &abnNodes) {
      int ctNodes = abnNodes.Count();

      for (int iNode = 0; iNode < ctNodes; iNode++) {
        AddReference(&abnNodes[iNode]);
      }

      return ctNodes;
    };

    // Print out the node
    string Print(string strTab) {
      // open this node block
      string str = strTab + _astrBuildNodes[lt_eType];
      str += " =={\n";

      string strNodeTab = strTab + ". ";

      // print node value
      string strValue = lt_valValue->Print();

      // special values
      switch (lt_eType) {
        case EBN_BINARY_OP:
        case EBN_ASSIGN_OP:
          strValue = _astrBinaryOps[lt_valValue->GetIndex()];
          break;

        case EBN_UNARY_OP:
          strValue = _astrUnaryOps[lt_valValue->GetIndex()];
          break;
      }

      // surround strings with quotes
      if (lt_valValue->GetType() == EVT_STRING) {
        strValue = "\"" + strValue + "\"";
      }

      str += LdsPrintF("%sValue : %s\n", strNodeTab.c_str(), strValue.c_str());
      
      // print node argument
      string strArg = _astrBuildNodeArguments[lt_eType];

      if (strArg != "") {
        str += LdsPrintF("%s%s : %d\n", strNodeTab.c_str(), strArg.c_str(), lt_iArg);
      }

      // open nodes block
      if (bn_abnNodes.Count() > 0) {
        str += '\n';
        str += strNodeTab + "NODES -{\n";
      }

      // print each sub node
      for (int iNode = 0; iNode < bn_abnNodes.Count(); iNode++) {
        if (iNode != 0) {
          str += '\n';
        }

        str += bn_abnNodes[iNode]->Print(strNodeTab + ". ");
      }

      // close nodes block
      if (bn_abnNodes.Count() > 0) {
        str += strNodeTab + "}-\n";
      }
      
      // close this node block
      str += strTab + "}==\n";

      return str;
    };
};
