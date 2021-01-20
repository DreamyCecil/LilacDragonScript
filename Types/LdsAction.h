#pragma once

#include "LdsToken.h"
#include "../Functions/LdsFunc.h"

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

// Compiler action
class LDS_API CCompAction : public CLdsToken {
  public:
    SLdsInlineFunc ca_inFunc; // inline function
    
    // Constructors
    CCompAction(void) : CLdsToken() {};
    
    CCompAction(const int &iType, const int &iLine, const int &iArg) :
      CLdsToken(iType, iLine, -1) {};
      
    CCompAction(const int &iType, const int &iLine, const SLdsValue &val, const int &iArg) :
      CLdsToken(iType, iLine, val, iArg) {};
};
