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

#include "StdH.h"

// Build the script or the expression
void CLdsScriptEngine::LdsBuild(bool bExpression) {
  _bnNode = CBuildNode();
  _iBuildPos = 0;
  _ctBuildLen = _aetTokens.Count() - 1;

  _bBuildBreak = false;
  _bBuildCont = false;
  
  _mapInlineFunc.Clear();

  // expression building
  if (bExpression) {
    ExpressionBuilder(LBF_NONE);

    if (_iBuildPos < _ctBuildLen - 1) {
      string strPos = _aetTokens[_iBuildPos].PrintPos();
      LdsThrow(LEB_STOPPED, "Stopped building at %s (%d tokens out of %d)", strPos.c_str(), _iBuildPos + 1, _ctBuildLen);
    }
    return;
  }

  // script building
  CNodeList abnNodes;

  while (_iBuildPos < _ctBuildLen) {
    // parse statement
    StatementBuilder();

    // add the statement
    abnNodes.Add(_bnNode);
  }

  // mark last node as a block of code
  _bnNode = CBuildNode(EBN_BLOCK, -1, -1, abnNodes.Count(), &abnNodes);
};

// Build one statement
void CLdsScriptEngine::StatementBuilder(void) {
  if (_ctBuildLen <= 0) {
    LdsThrow(LEB_EMPTY, "No parser tokens");
  }

  const CLdsToken &et = _aetTokens[_iBuildPos++];

  switch (et.lt_eType) {
    // return from the script with the value
    case LTK_RETURN: {
      const CLdsToken &etNext = _aetTokens[_iBuildPos];
      
      // no return value
      if (etNext.lt_eType == LTK_SEMICOL) {
        _bnNode = CBuildNode(EBN_RETURN, et.lt_iPos, -1, false);
        break;
      }
      
      ExpressionBuilder(LBF_NONE);
      CBuildNode bnExp = _bnNode;

      _bnNode = CBuildNode(EBN_RETURN, et.lt_iPos, -1, true);
      _bnNode.AddReference(&bnExp);
    } break;
    
    // if condition
    case LTK_IF: {
      ExpressionBuilder(LBF_NONE);
      CBuildNode bnCond = _bnNode;
    
      StatementBuilder();
      CBuildNode bnThen = _bnNode;

      const CLdsToken &etNext = _aetTokens[_iBuildPos];
    
      // else condition
      if (etNext.lt_eType == LTK_ELSE) {
        _iBuildPos++;
      
        StatementBuilder();
        CBuildNode bnElse = _bnNode;
      
        _bnNode = CBuildNode(EBN_IF_THEN_ELSE, etNext.lt_iPos, -1, -1);
        _bnNode.AddReference(&bnCond);
        _bnNode.AddReference(&bnThen);
        _bnNode.AddReference(&bnElse);
      
      } else {
        _bnNode = CBuildNode(EBN_IF_THEN, etNext.lt_iPos, -1, -1);
        _bnNode.AddReference(&bnCond);
        _bnNode.AddReference(&bnThen);
      }
    } break;
    
    // switch-case block
    case LTK_SWITCH: {
      ExpressionBuilder(LBF_NONE);
      CBuildNode bnExp = _bnNode;
    
      CLdsToken etNext = _aetTokens[_iBuildPos++];

      if (etNext.lt_eType != LTK_CUR_OPEN) {
        LdsThrow(LEB_OPENCB, "Expected a '{' at %s", etNext.PrintPos().c_str());
      }
    
      CNodeList abnCases;
      CNodeList abnActions;
    
      CBuildNode bnDef;
      bool bSetDefault = false;
      bool bClosed = false;
    
      while (_iBuildPos < _ctBuildLen) {
        etNext = _aetTokens[_iBuildPos++];
      
        if (etNext.lt_eType == LTK_CUR_CLOSE) {
          bClosed = true;
          break;
        
        } else if (etNext.lt_eType == LTK_CASE || etNext.lt_eType == LTK_DEFAULT) {
          // actions node and an action counter
          CBuildNode *pbnActions = NULL;
          int ctNodes = 0;
        
          // case option
          if (etNext.lt_eType == LTK_CASE) {
            // read case value
            ExpressionBuilder(LBF_NONE);
            abnCases.Add(_bnNode);
            
            // empty case actions
            int iActions = abnActions.Add(CBuildNode(EBN_BLOCK, etNext.lt_iPos, -1, -1));
            pbnActions = &abnActions[iActions];
          
          // default option
          } else {
            // already set
            if (bSetDefault) {
              LdsThrow(LEB_DEFAULT, "Cannot have another 'default' option at %s", etNext.PrintPos().c_str());
            }
            
            // empty default actions
            bnDef = CBuildNode(EBN_BLOCK, etNext.lt_iPos, -1, -1);
            pbnActions = &bnDef;
            
            bSetDefault = true;
          }
        
          etNext = _aetTokens[_iBuildPos++];
        
          if (etNext.lt_eType != LTK_COLON) {
            LdsThrow(LEB_COLON, "Expected a ':' at %s", etNext.PrintPos().c_str());
          }
        
          // read statements until another case
          while (_iBuildPos < _ctBuildLen) {
            etNext = _aetTokens[_iBuildPos];
            
            // hit the end, another case or default option
            if (etNext.lt_eType == LTK_CUR_CLOSE
             || etNext.lt_eType == LTK_CASE
             || etNext.lt_eType == LTK_DEFAULT) {
              break;
            }
            
            // add statement to the node of case actions
            BuildSwitchCaseBody();
            pbnActions->AddReference(&_bnNode);
            ctNodes++;
          }
          
          // count added statements
          pbnActions->lt_iArg = ctNodes;
          
        } else {
          LdsThrow(LEB_SWITCH, "Expected a 'case' or '}' at %s", etNext.PrintPos().c_str());
        }
      }
    
      _bnNode = CBuildNode(EBN_SWITCH, et.lt_iPos, -1, -1);
      
      // make lists of arguments and options
      CBuildNode bnCasesList = CBuildNode(EBN_BLOCK, et.lt_iPos, -1, abnCases.Count(), &abnCases);
      CBuildNode bnActionsList = CBuildNode(EBN_BLOCK, et.lt_iPos, -1, abnActions.Count(), &abnActions);

      _bnNode.AddReference(&bnExp);
      _bnNode.AddReference(&bnCasesList);
      _bnNode.AddReference(&bnActionsList);

      // add default option if possible
      if (bSetDefault) {
        _bnNode.AddReference(&bnDef);
      }
    } break;
    
    // block of statements
    case LTK_CUR_OPEN: {
      CNodeList abnNodes;
      bool bClosed = false;
    
      // build statement within the curly brackets
      while (_iBuildPos < _ctBuildLen) {
        const CLdsToken &etNext = _aetTokens[_iBuildPos];
      
        // block end
        if (etNext.lt_eType == LTK_CUR_CLOSE) {
          _iBuildPos++;
          bClosed = true;
          break;
        }
      
        // build next statement
        StatementBuilder();
        abnNodes.Add(_bnNode);
      }
    
      if (!bClosed) {
        LdsThrow(LEB_CLOSECB, "Unclosed code block starting at %s", et.PrintPos().c_str());
      }
    
      _bnNode = CBuildNode(EBN_BLOCK, et.lt_iPos, -1, abnNodes.Count(), &abnNodes);
    } break;
    
    // while loop
    case LTK_WHILE: {
      ExpressionBuilder(LBF_NONE);
      CBuildNode bnCond = _bnNode;
    
      BuildLoopBody();
      CBuildNode bnLoop = _bnNode;

      _bnNode = CBuildNode(EBN_WHILE, et.lt_iPos, -1, -1);
      _bnNode.AddReference(&bnCond);
      _bnNode.AddReference(&bnLoop);
    } break;
    
    // do-while loop
    case LTK_DO: {
      BuildLoopBody();
      CBuildNode bnLoop = _bnNode;
    
      // while check
      const CLdsToken &etNext = _aetTokens[_iBuildPos];
    
      if (etNext.lt_eType != LTK_WHILE) {
        LdsThrow(LEB_WHILE, "Expected a 'while' after 'do' block at %s", etNext.PrintPos().c_str());
      }
    
      _iBuildPos++;
    
      // condition
      ExpressionBuilder(LBF_NONE);
      CBuildNode bnCond = _bnNode;
    
      _bnNode = CBuildNode(EBN_DO_WHILE, et.lt_iPos, -1, -1);
      _bnNode.AddReference(&bnLoop);
      _bnNode.AddReference(&bnCond);
    } break;
    
    // for loop
    case LTK_FOR: {
      // see if there's a parenthesis
      CLdsToken etNext = _aetTokens[_iBuildPos];
    
      bool bPar = (etNext.lt_eType == LTK_PAR_OPEN);

      if (bPar) {
        _iBuildPos++;
      }
    
      // initialization
      StatementBuilder();
      CBuildNode bnInit = _bnNode;
    
      // condition
      ExpressionBuilder(LBF_NONE);
      CBuildNode bnCond = _bnNode;

      etNext = _aetTokens[_iBuildPos];
    
      if (etNext.lt_eType == LTK_SEMICOL) {
        _iBuildPos++;
      }
    
      // post-statement
      StatementBuilder();
      CBuildNode bnPost = _bnNode;
    
      // see if there's a closing parenthesis
      if (bPar) {
        etNext = _aetTokens[_iBuildPos];
      
        if (etNext.lt_eType != LTK_PAR_CLOSE) {
          LdsThrow(LEB_CLOSEP, "Expected a ')' at %s", etNext.PrintPos().c_str());
        }
      
        _iBuildPos++;
      }
    
      // loop body
      BuildLoopBody();
      CBuildNode bnLoop = _bnNode;
    
      _bnNode = CBuildNode(EBN_FOR, et.lt_iPos, -1, -1);
      _bnNode.AddReference(&bnInit);
      _bnNode.AddReference(&bnCond);
      _bnNode.AddReference(&bnPost);
      _bnNode.AddReference(&bnLoop);
    } break;
    
    // break from this statement
    case LTK_BREAK: {
      if (_bBuildBreak) {
        _bnNode = CBuildNode(EBN_BREAK, et.lt_iPos, -1, -1);
      
      } else {
        LdsThrow(LEB_BREAK, "Can't 'break' at %s", et.PrintPos().c_str());
      }
    } break;
    
    // continue to the next iteration
    case LTK_CONTINUE: {
      if (_bBuildCont) {
        _bnNode = CBuildNode(EBN_CONTINUE, et.lt_iPos, -1, -1);
      
      } else {
        LdsThrow(LEB_CONTINUE, "Can't 'continue' at %s", et.PrintPos().c_str());
      }
    } break;
    
    default:
      _iBuildPos--;
      
      // build definitions
      if (DefinitionBuilder()) {
        break;
      }
      _iBuildPos--;

      ExpressionBuilder(LBF_NOOPS);
      CBuildNode bnExp = _bnNode;
    
      switch (bnExp.lt_eType) {
        case EBN_PREFIX:
        case EBN_POSTFIX:
          _bnNode.lt_eType = EBN_ADJFIX;
          break;
        
        case EBN_CALL:
          // discard return value if the function is called as a statement
          _bnNode = CBuildNode(EBN_DISCARD, bnExp.lt_iPos, -1, -1);
          _bnNode.AddReference(&bnExp);
          break;
        
        default:
          const CLdsToken &etNext = _aetTokens[_iBuildPos];
        
          // assignment
          if (etNext.lt_eType == LTK_SET) {
            _iBuildPos++;
          
            ExpressionBuilder(LBF_NONE);
            CBuildNode bnSet = _bnNode;
          
            _bnNode = CBuildNode(EBN_SET, etNext.lt_iPos, etNext.lt_valValue, -1);
            _bnNode.AddReference(&bnExp);
            _bnNode.AddReference(&bnSet);
          
          } else {
            LdsThrow(LEB_STATEMENT, "Expected a statement at %s", _bnNode.PrintPos().c_str());
          }
      }
  }

  // allow a semicolon after statements
  const CLdsToken &etSemicolon = _aetTokens[_iBuildPos];

  if (etSemicolon.lt_eType == LTK_SEMICOL) {
    _iBuildPos++;
  }
};

// Build definitions
bool CLdsScriptEngine::DefinitionBuilder(void) {
  const CLdsToken &et = _aetTokens[_iBuildPos++];
  
  switch (et.lt_eType) {
    // variable definition
    case LTK_VAR: {
      CNodeList abnNodes;
      bool bConst = et->IsTrue();
      
      do {
        CLdsToken etNext = _aetTokens[_iBuildPos++];
      
        if (etNext.lt_eType != LTK_ID) {
          LdsThrow(LEB_ID, "Expected a variable name at %s", etNext.PrintPos().c_str());
        }
      
        string strName = etNext->GetString();
        
        // define the variable
        CBuildNode bnDefine = CBuildNode(EBN_VAR, etNext.lt_iPos, strName, bConst);
        abnNodes.Add(bnDefine);
      
        etNext = _aetTokens[_iBuildPos];
        CBuildNode bnExp;
      
        // check for value
        if (etNext.lt_eType == LTK_SET) {
          etNext = _aetTokens[++_iBuildPos];
        
          ExpressionBuilder(LBF_NONE);
          bnExp = _bnNode;

          // assign a value
          CBuildNode bnSetVar = CBuildNode(EBN_SET, etNext.lt_iPos, LOP_SET, -1);
          CBuildNode bnVar = CBuildNode(EBN_ID, etNext.lt_iPos, strName, 0);

          bnSetVar.AddReference(&bnVar);
          bnSetVar.AddReference(&bnExp);
        
          // assign value to this variable
          abnNodes.Add(bnSetVar);
        }
      
        // check for a comma
        etNext = _aetTokens[_iBuildPos];
      
        if (etNext.lt_eType != LTK_COMMA) {
          break;
        }
      
        _iBuildPos++;
      
      } while (_iBuildPos < _ctBuildLen);
    
      _bnNode = CBuildNode(EBN_BLOCK, et.lt_iPos, -1, abnNodes.Count(), &abnNodes);
    } break;
    
    // function definition
    case LTK_FUNC: {
      CLdsToken etNext = _aetTokens[_iBuildPos++];
    
      if (etNext.lt_eType != LTK_ID) {
        LdsThrow(LEB_ID, "Expected a function name at %s", etNext.PrintPos().c_str());
      }
      
      // get function name
      string strFunc = etNext->GetString();
      
      // expect function arguments
      etNext = _aetTokens[_iBuildPos++];
      
      if (etNext.lt_eType != LTK_PAR_OPEN) {
        LdsThrow(LEB_OPENP, "Expected a '(' at %s", etNext.PrintPos().c_str());
      }
      
      // function arguments
      CLdsInlineArgs astrArgs;
      bool bClosed = false;

      while (_iBuildPos < _ctBuildLen) {
        etNext = _aetTokens[_iBuildPos++];

        // function arguments closing
        if (etNext.lt_eType == LTK_PAR_CLOSE) {
          bClosed = true;
          break;
        }

        // expect argument name
        if (etNext.lt_eType != LTK_ID) {
          LdsThrow(LEB_ID, "Expected an argument name at %s", etNext.PrintPos().c_str());
        }
        
        astrArgs.Add(etNext->GetString());

        // skip commas
        etNext = _aetTokens[_iBuildPos];

        if (etNext.lt_eType == LTK_COMMA) {
          _iBuildPos++;

        // expect a closing parenthesis
        } else if (etNext.lt_eType != LTK_PAR_CLOSE) {
          LdsThrow(LEB_COMMA, "Expected ',' or ')' at %s", etNext.PrintPos().c_str());
        }
      }

      if (!bClosed) {
        LdsThrow(LEB_CLOSEP, "Unclosed function arguments at %s", et.PrintPos().c_str());
      }
      
      /* TODO: Since inline functions can be nested, they are added for the whole thread and can be called but won't have actions
               because those are added for a specific function while compiling. Not much of a problem, but may be confusing.
               
        Example:
      
        // nested functions
        function func() {
          return inlineFunc();
          
          // print and return "Hello"
          function inlineFunc() {
            return Print("Hello");
          }
        }
        
        var test1 = func(); // prints "Hello"
        var test2 = inlineFunc(); // doesn't do anything and returns 0
        
        return (test1 == test2); // returns 'false'
      */
      
      // function already exists
      if (_mapInlineFunc.FindKeyIndex(strFunc) != -1) {
        LdsThrow(LEB_FUNCDEF, "Inline function '%s' redefinition at %s", strFunc.c_str(), et.PrintPos().c_str());
      }
      
      // add a function reference with no actions
      _mapInlineFunc[strFunc] = SLdsInlineFunc(astrArgs, CActionList());
    
      // TODO: Make a new variable '_bInlineFunc' and set it to 'true' here, so inline functions of other
      //       inline functions nested as well instead of being put in one global '_mapInlineFunc' map.
      //       Otherwise they can't be redefined and can be called outside of the current thread.
      
      // build function body
      StatementBuilder();
      CBuildNode bnBody = _bnNode;
      
      _bnNode = CBuildNode(EBN_FUNC, et.lt_iPos, strFunc, -1);
      _bnNode.AddReference(&bnBody);
    } break;
    
    // thread directive
    case LTK_DIR: {
      // directive type
      int iDirType = et->GetIndex();
      
      // get directive value
      const CLdsToken &etNext = _aetTokens[_iBuildPos++];
    
      if (etNext.lt_eType != LTK_VAL) {
        LdsThrow(LEB_VALUE, "Expected a value at %s", etNext.PrintPos().c_str());
      }
      
      CLdsValue valDir = etNext.lt_valValue;
      
      // assert value type
      int iDesiredVal = -1;
      
      switch (iDirType) {
        case THD_DEBUGCONTEXT: iDesiredVal = EVT_INDEX; break;
      }
      
      // wrong type
      if (iDesiredVal != -1 && valDir->GetType() != iDesiredVal) {
        string strDesired = TypeName((ELdsValueType)iDesiredVal, "number", "string", "", "");
        string strValType = valDir->TypeName("number", "string", "", "");
        
        LdsThrow(LER_TYPE, "Expected a %s but got a %s at %s", strDesired.c_str(), strValType.c_str(), etNext.PrintPos().c_str());
      }
      
      _bnNode = CBuildNode(EBN_DIR, et.lt_iPos, valDir, iDirType);
    } break;
    
    // unknown token
    default: return false;
  }
  
  return true;
};

// Build one expression
void CLdsScriptEngine::ExpressionBuilder(const LdsFlags &ubFlags) {
  if (_ctBuildLen <= 0) {
    LdsThrow(LEB_EMPTY, "No parser tokens");
  }

  const CLdsToken &et = _aetTokens[_iBuildPos];
  
  // build scope identifiers (advances position)
  if (!ScopeBuilder()) {
    // only identifiers are allowed
    if (ubFlags & LBF_SCOPE) {
      LdsThrow(LEB_ID, "Expected identifier at %s", et.PrintPos().c_str());
    }

    switch (et.lt_eType) {
      // values
      case LTK_VAL: {
        CLdsValue val = et.lt_valValue;
        
        // go through tokens if it's a string value
        if (val->GetType() == EVT_STRING)
        {
          while (_iBuildPos < _ctBuildLen) {
            // get token after the string
            CLdsToken etNext = _aetTokens[_iBuildPos];
          
            // if followed by another string
            if (etNext.lt_eType == LTK_VAL && etNext->GetType() == EVT_STRING) {
              // concatenate them
              val = val->Print() + etNext->Print();
            
            // something else
            } else {
              break;
            }

            _iBuildPos++;
          }
        }

        _bnNode = CBuildNode(EBN_VAL, et.lt_iPos, val, -1);
      } break;

      // arrays
      case LTK_SQ_OPEN: {
        CNodeList abnArray;
      
        // read arguments and the closing bracket
        bool bClosed = false;
      
        while (_iBuildPos < _ctBuildLen) {
          // check if reached the closing bracket
          CLdsToken etNext = _aetTokens[_iBuildPos];
        
          if (etNext.lt_eType == LTK_SQ_CLOSE) {
            _iBuildPos++;
            bClosed = true;
            break;
          }
        
          // read the value
          ExpressionBuilder(LBF_NONE);
          CBuildNode bnVal = _bnNode;
        
          abnArray.Add(bnVal);
        
          // skip the comma
          etNext = _aetTokens[_iBuildPos];
        
          if (etNext.lt_eType == LTK_COMMA) {
            _iBuildPos++;
          
          } else if (etNext.lt_eType != LTK_SQ_CLOSE) {
            LdsThrow(LEB_ARRAY, "Expected a comma or a ']' at %s", etNext.PrintPos().c_str());
          }
        }
      
        if (!bClosed) {
          LdsThrow(LEB_CLOSESB, "Unclosed array at %s", et.PrintPos().c_str());
        }
      
        // create an array
        _bnNode = CBuildNode(EBN_ARRAY, et.lt_iPos, -1, abnArray.Count(), &abnArray);
      } break;
      
      // structure
      case LTK_STATIC:
      case LTK_CUR_OPEN: {
        CNodeList abnVars;
        int iType = 1;
        
        // mark as static
        if (et.lt_eType == LTK_STATIC) {
          iType = 2;
          
          // expect structure opening
          const CLdsToken &etOpening = _aetTokens[_iBuildPos++];
          
          if (etOpening.lt_eType != LTK_CUR_OPEN) {
            LdsThrow(LEB_OPENCB, "Expected a '{' at %s", etOpening.PrintPos().c_str());
          }
        }
      
        // read arguments and the closing bracket
        bool bClosed = false;
      
        while (_iBuildPos < _ctBuildLen) {
          // check if reached the closing bracket
          CLdsToken etNext = _aetTokens[_iBuildPos++];
        
          if (etNext.lt_eType == LTK_CUR_CLOSE) {
            bClosed = true;
            break;
          }
          
          // optional variable definition
          bool bConst = false;
          
          if (etNext.lt_eType == LTK_VAR) {
            bConst = etNext->IsTrue();
            
            etNext = _aetTokens[_iBuildPos++];
          }
          
          // read the variable name
          if (etNext.lt_eType != LTK_ID) {
            LdsThrow(LEB_ID, "Expected a variable name at %s", etNext.PrintPos().c_str());
          }
          
          CBuildNode bnStructVar = CBuildNode(EBN_SVAR, etNext.lt_iPos, etNext.lt_valValue, bConst);
          
          // assignment operator
          etNext = _aetTokens[_iBuildPos++];
          
          if (etNext.lt_eType != LTK_SET && etNext->GetIndex() != LOP_SET) {
            LdsThrow(LEB_ASSIGN, "Expected a '=' at %s", etNext.PrintPos().c_str());
          }
        
          // read the value and add it to the variable
          ExpressionBuilder(LBF_NONE);
          bnStructVar.AddReference(&_bnNode);
        
          // add one structure variable
          abnVars.Add(bnStructVar);
        
          // skip the comma
          etNext = _aetTokens[_iBuildPos];
        
          if (etNext.lt_eType == LTK_COMMA) {
            _iBuildPos++;
          
          } else if (etNext.lt_eType != LTK_CUR_CLOSE) {
            LdsThrow(LEB_STRUCT, "Expected a comma or a '}' at %s", etNext.PrintPos().c_str());
          }
        }
      
        if (!bClosed) {
          LdsThrow(LEB_CLOSECB, "Unclosed structure at %s", et.PrintPos().c_str());
        }
      
        // create a structure
        _bnNode = CBuildNode(EBN_STRUCT, et.lt_iPos, iType, abnVars.Count(), &abnVars);
      } break;
        
      // inside the parentheses
      case LTK_PAR_OPEN: {
        ExpressionBuilder(LBF_NONE);
        const CLdsToken &etClosing = _aetTokens[_iBuildPos++];

        if (etClosing.lt_eType != LTK_PAR_CLOSE) {
          LdsThrow(LEB_CLOSEP, "Expected ')' at %s", etClosing.PrintPos().c_str());
        }
      } break;

      // binary/unary operation
      case LTK_OPERATOR: {
        switch (et->GetIndex()) {
          case LOP_ADD:
            ExpressionBuilder(LBF_NOOPS);
            break;

          case LOP_SUB: {
            ExpressionBuilder(LBF_NOOPS);
            CBuildNode bnNum = _bnNode;

            _bnNode = CBuildNode(EBN_UN, et.lt_iPos, UOP_NEGATE, -1);
            _bnNode.AddReference(&bnNum);
          } break;

          default: LdsThrow(LEB_OPERATOR, "Unexpected operator token at %s", et.PrintPos().c_str());
        }
      } break;

      // unary operation
      case LTK_UNARYOP: {
        ExpressionBuilder(LBF_NOOPS);
        CBuildNode bnNum = _bnNode;

        _bnNode = CBuildNode(EBN_UN, et.lt_iPos, et.lt_valValue, -1);
        _bnNode.AddReference(&bnNum);
      } break;

      // prefix operation
      case LTK_ADJFIX: {
        ExpressionBuilder(LBF_NOOPS);
        CBuildNode bnNum = _bnNode;

        _bnNode = CBuildNode(EBN_PREFIX, et.lt_iPos, et.lt_valValue, -1);
        _bnNode.AddReference(&bnNum);
      } break;

      default: LdsThrow(LEB_TOKEN, "Unexpected token at %s", et.PrintPos().c_str());
    }
  }

  // postfix operations
  PostfixBuilder(false);

  // build next operation
  if (!(ubFlags & LBF_NOOPS)) {
    const CLdsToken &etOperator = _aetTokens[_iBuildPos];

    if (etOperator.lt_eType == LTK_OPERATOR) {
      _iBuildPos++;
      OperationBuilder(etOperator);
    }
  }
};

// Build identifiers
bool CLdsScriptEngine::ScopeBuilder(void) {
  const CLdsToken &et = _aetTokens[_iBuildPos++];
  
  // variables or functions
  switch (et.lt_eType) {
    case LTK_ID: {
      CLdsToken etNext = _aetTokens[_iBuildPos];

      // function opening
      if (etNext.lt_eType == LTK_PAR_OPEN) {
        _iBuildPos++;

        CNodeList abnArgs;
        bool bClosed = false;

        while (_iBuildPos < _ctBuildLen) {
          etNext = _aetTokens[_iBuildPos];

          // function closing
          if (etNext.lt_eType == LTK_PAR_CLOSE) {
            _iBuildPos++;
            bClosed = true;
            break;
          }

          // read one argument
          ExpressionBuilder(LBF_NONE);

          // save it
          abnArgs.Add(_bnNode);

          // skip commas
          etNext = _aetTokens[_iBuildPos];

          if (etNext.lt_eType == LTK_COMMA) {
            _iBuildPos++;

          // expect a closing parenthesis
          } else if (etNext.lt_eType != LTK_PAR_CLOSE) {
            LdsThrow(LEB_FUNCTION, "Expected ',' or ')' at %s", etNext.PrintPos().c_str());
          }
        }

        if (!bClosed) {
          LdsThrow(LEB_CLOSEP, "Unclosed function at %s", et.PrintPos().c_str());
        }
        
        _bnNode = CBuildNode(EBN_CALL, et.lt_iPos, et.lt_valValue, abnArgs.Count(), &abnArgs);

      // variable
      } else {
        _bnNode = CBuildNode(EBN_ID, et.lt_iPos, et.lt_valValue, -1);
      }
    } break;
    
    // unknown token
    default: return false;
  }
  
  return true;
};

// Build postfix operations
bool CLdsScriptEngine::PostfixBuilder(bool bChained) {
  const CLdsToken &et = _aetTokens[_iBuildPos++];
  
  // built value/variable
  CBuildNode bnVal = _bnNode;
  
  switch (et.lt_eType) {
    // iVal++
    case LTK_ADJFIX: {
      // not a chained accessor
      if (bChained) {
        _iBuildPos--;
        return false;
      }

      _bnNode = CBuildNode(EBN_POSTFIX, et.lt_iPos, et.lt_valValue, -1);
      _bnNode.AddReference(&bnVal);
    } break;
    
    // aArr[value][...]
    case LTK_SQ_OPEN:
    // sStruct.variable
    case LTK_ACCESS: {
      CBuildNode bnID;
      
      if (et.lt_eType == LTK_SQ_OPEN) {
        // read the index
        ExpressionBuilder(LBF_NONE);
        bnID = _bnNode;

        // closing bracket
        const CLdsToken &etNext = _aetTokens[_iBuildPos++];

        if (etNext.lt_eType != LTK_SQ_CLOSE) {
          LdsThrow(LEB_CLOSESB, "Unclosed '[]' at %s", etNext.PrintPos().c_str());
        }
      } else {
        // identifier name
        const CLdsToken &etNext = _aetTokens[_iBuildPos++];

        if (etNext.lt_eType != LTK_ID) {
          LdsThrow(LEB_ID, "Expected a variable name at %s", etNext.PrintPos().c_str());
        }
        
        bnID = CBuildNode(EBN_VAL, etNext.lt_iPos, etNext.lt_valValue, -1);
      }
      
      CBuildNode bnAccess = CBuildNode(EBN_ACCESS, et.lt_iPos, bnVal.lt_valValue, false);
      
      if (bChained) {
        CBuildNode bnNone = CBuildNode(EBN_DISCARD, _bnNode.lt_iPos, -1, -1);
        bnAccess.AddReference(&bnNone);
      } else {
        bnAccess.AddReference(&bnVal);
      }

      bnAccess.AddReference(&bnID);
      
      // chained accessors
      if (_iBuildPos < _ctBuildLen) {
        if (PostfixBuilder(true)) {
          bnAccess.AddReference(&_bnNode);
        }
      }
      
      _bnNode = bnAccess;

      // parse the rest of the postfixes
      if (!bChained) {
        PostfixBuilder(false);
      }
    } break;
    
    // no postfix
    default: {
      _iBuildPos--;
    } return false;
  }
  
  return true;
};

// Build one operation
void CLdsScriptEngine::OperationBuilder(CLdsToken etFirst) {
  CNodeList abnNodes;
  abnNodes.Add(_bnNode);

  CTokenList aetOperators;
  aetOperators.Add(etFirst);

  CLdsToken et;

  while (true) {
    // build next expression
    ExpressionBuilder(LBF_NOOPS);
    abnNodes.Add(_bnNode);

    et = _aetTokens[_iBuildPos];

    // add operation to the list
    if (et.lt_eType == LTK_OPERATOR) {
      _iBuildPos++;
      aetOperators.Add(et);

    } else {
      break;
    }
  }

  int ctOperations = aetOperators.Count();
  int iMaxPriority = (LOP_MAX >> 4);
  int iPriority = 0;

  // sort operations by the priority
  while (iPriority < iMaxPriority) {
    for (int iOperator = 0; iOperator < ctOperations; iOperator++) {
      et = aetOperators[iOperator];

      // operator doesn't match the priority
      if ((et->GetIndex() >> 4) != iPriority) {
        continue;
      }

      // get two values for the operation
      CBuildNode &bnNode1 = abnNodes[iOperator];
      CBuildNode &bnNode2 = abnNodes[iOperator + 1];

      // if they are actually both pure values
      if (bnNode1.lt_eType == EBN_VAL && bnNode2.lt_eType == EBN_VAL) {
        // perform operation in place
        CLdsToken tknOp(LTK_OPERATOR, et.lt_iPos, et.lt_valValue, -1);

        CLdsValueRef valRef1(bnNode1.lt_valValue);
        CLdsValueRef valRef2(bnNode2.lt_valValue);

        valRef1 = valRef1.vr_val->BinaryOp(valRef1, valRef2, tknOp);

        // add pure value
        abnNodes[iOperator] = CBuildNode(EBN_VAL, et.lt_iPos, valRef1.vr_val, -1);

      // if has identifiers or other operations
      } else {
        // add binary operation
        CNodeList abn;
        abn.Add(bnNode1);
        abn.Add(bnNode2);

        abnNodes[iOperator] = CBuildNode(EBN_BIN, et.lt_iPos, et.lt_valValue, -1, &abn);
        abn.Clear();
      }

      // remove the other node with this operation
      abnNodes.Delete(iOperator + 1);
      aetOperators.Delete(iOperator);

      ctOperations--;
      iOperator--;
    }

    // increase priority
    iPriority++;
  }

  // return with an operation node
  _bnNode = abnNodes[0];

  abnNodes.Clear();
  aetOperators.Clear();
};

// Build loop body
void CLdsScriptEngine::BuildLoopBody(void) {
  bool bCouldBreak = _bBuildBreak;
  bool bCouldCont = _bBuildCont;
  _bBuildBreak = true;
  _bBuildCont = true;

  StatementBuilder();

  _bBuildBreak = bCouldBreak;
  _bBuildCont = bCouldCont;
};

// Build switch's case body
void CLdsScriptEngine::BuildSwitchCaseBody(void) {
  bool bCouldBreak = _bBuildBreak;
  _bBuildBreak = true;

  StatementBuilder();

  _bBuildBreak = bCouldBreak;
};
