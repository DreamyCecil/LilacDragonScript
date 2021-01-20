#pragma once

#include "LdsToken.h"

// Build node type
enum EBuildNode {
  EBN_VAL,    // value
  EBN_ARRAY,  // array of values (arg: array size)
  EBN_STRUCT, // structure with value fields (arg: amount of variables)
  EBN_ID,     // identifier

  EBN_UN,  // unary operator
  EBN_BIN, // binary operator
  EBN_SET, // assignment operator
  
  EBN_PREFIX,  // ++i
  EBN_POSTFIX, // i++
  EBN_ADJFIX,  // i += 1
  EBN_ACCESS,  // variable accessor
  
  // functions
  EBN_CALL,
  EBN_RETURN,
  EBN_DISCARD,
  EBN_FUNC, // inline function definition
  EBN_VAR,  // variable definition
  EBN_SVAR, // structure variable definition
  
  // blocks
  EBN_BLOCK, // (arg: amount of expressions)
  EBN_IF_THEN,
  EBN_IF_THEN_ELSE,
  EBN_SWITCH, // (arg: amount of options)
  
  // loops
  EBN_WHILE,
  EBN_DO_WHILE,
  EBN_FOR,
  EBN_BREAK,
  EBN_CONTINUE,
  
  EBN_DIR, // thread directive
};

// Unary operations
enum EUnaryOp {
  UOP_NEGATE,  // - negation
  UOP_INVERT,  // ! inversion
  UOP_BINVERT, // ~ bitwise inversion
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
    CBuildNode(EBuildNode eType, int iPos, SLdsValue valValue, int iArg, CNodeList *abnNodes = NULL) :
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

    // Copy nodes from some list
    int CopyNodes(const CDynamicNodeList &abnNodes) {
      int ctNodes = abnNodes.Count();

      for (int iNode = 0; iNode < ctNodes; iNode++) {
        AddReference(abnNodes[iNode]);
      }

      return ctNodes;
    };
    int CopyNodes(CNodeList &abnNodes) {
      int ctNodes = abnNodes.Count();

      for (int iNode = 0; iNode < ctNodes; iNode++) {
        AddReference(&abnNodes[iNode]);
      }

      return ctNodes;
    };
};
