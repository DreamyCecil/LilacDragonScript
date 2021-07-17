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

#include "LdsTypes.h"

// Script cache
struct LDS_API SLdsCache {
  CActionList acaCache; // list of actions
  bool bExpression; // not a whole script

  // Constructors
  SLdsCache(void) : bExpression(false) {};

  SLdsCache(const CActionList &acaSetActions, const bool &bSetExpression) {
    acaCache.CopyArray(acaSetActions);
    bExpression = bSetExpression;
  };
};

class LDS_API CLdsScriptEngine {
  // Compatibility
  public:
    // Custom output functions
    void (*_pLdsPrintFunction)(const char *);
    void (*_pLdsErrorFunction)(const char *);
    // A function to call after destruction
    void (*_pDestructorFunc)(CLdsScriptEngine *plds);

    // Set output printing functions
    void LdsOutputFunctions(void *pPrint, void *pError);
    
    // Print out formatted string
    void LdsOut(const char *strFormat, ...);
    // Print out an error
    void LdsErrorOut(const char *strFormat, ...);
    
  // I/O
  public:
    // Script loading function
    bool (*_pLdsLoadScript)(const char *strFile, string &strScript);

    // Write some data into a data stream
    void (*_pLdsWrite)(void *pStream, const void *pData, const LdsSize &iSize);
    // Read some data from a data stream
    void (*_pLdsRead)(void *pStream, void *pData, const LdsSize &iSize);

    // Get current position in a data stream
    int (*_pLdsStreamTell)(void *pStream);

    // Set stream functions
    void LdsStreamFunctions(void *pWrite, void *pRead, void *pTell);

    // Compiled scripts I/O

    // Write and read programs
    void LdsWriteProgram(void *pStream, CActionList &acaProgram);
    void LdsReadProgram(void *pStream, CActionList &acaProgram);

    // Write and read actions
    void LdsWriteAction(void *pStream, CCompAction &caAction);
    void LdsReadAction(void *pStream, CCompAction &caAction);

    // Write and read inline functions
    void LdsWriteInlineFunc(void *pStream, SLdsInlineFunc &inFunc);
    void LdsReadInlineFunc(void *pStream, SLdsInlineFunc &inFunc);

    // Script values I/O

    // Write and read values
    void LdsWriteValue(void *pStream, CLdsValue &val);
    void LdsReadValue(void *pStream, CLdsValue &val);
    
    // Write and read value references
    void LdsWriteValueRef(void *pStream, CLdsThread &sth, CLdsValueRef &vr);
    void LdsReadValueRef(void *pStream, CLdsThread &sth, CLdsValueRef &vr);
    
    // Write and read strings
    void LdsWriteString(void *pStream, string &str);
    void LdsReadString(void *pStream, string &str);

    // Current scripts I/O

    // Write and read the script engine (always called before thread I/O)
    void LdsWriteEngine(void *pStream);
    void LdsReadEngine(void *pStream);

    // Write and read threads
    void LdsWriteThread(void *pStream, CLdsThread &sth, bool bHandler);
    void LdsReadThread(void *pStream, CLdsThread &sth, bool bHandler);
    
  // Functions
  public:
    CLdsFuncMap _mapLdsDefFunc; // default functions
    CLdsFuncMap _mapLdsFunctions; // custom functions
    
    // Set default functions
    void SetDefaultFunctions(void);
    // Set custom functions from the map
    void SetCustomFunctions(CLdsFuncMap &mapFrom);
    // Add more functions and replace ones that already exist
    void AddCustomFunctions(CLdsFuncMap &mapFrom);

    // Call function from the action
    LdsReturn CallFunction(CCompAction *pcaAction, CLdsValueList &avalArgs);
    
  // Variables
  public:
    CLdsVarMap _mapLdsDefVar; // default variables
    CLdsVarMap _mapLdsVariables; // custom variables (used in I/O)
    
    // Set default variables
    void SetDefaultVariables(void);
    // Set custom variables from the map
    void SetCustomVariables(CLdsVarMap &mapFrom);
    // Add more variables and replace ones that already exist
    void AddCustomVariables(CLdsVarMap &mapFrom);
    
    // Get a variable by name
    bool LdsVariableValue(const string &strVar, SLdsVar *&pvar);
  
  // Parser
  public:
    CLdsValueMap _mapLdsConstants; // custom constants
    
    // Set custom constants
    void SetParserConstants(CLdsValueMap &mapFrom);
    
  private:
    CTokenList _aetTokens; // tokens from the script

    // Parse the script
    void ParseScript(string strScript);

    // Add one token to the list
    void AddParserToken(const ELdsToken &eType, const int &iPos);
    void AddParserToken(const ELdsToken &eType, const int &iPos, const int &iValue);
    void AddParserToken(const ELdsToken &eType, const int &iPos, const double &dValue);
    void AddParserToken(const ELdsToken &eType, const int &iPos, const string &strValue);
    void AddParserToken(const ELdsToken &eType, const int &iPos, const CLdsValue &valValue);
    
  // Builder
  private:
    CBuildNode _bnNode; // current build node
    int _iBuildPos; // current node index
    int _ctBuildLen; // amount of nodes

    bool _bBuildBreak; // can break
    bool _bBuildCont; // can continue
    
    CLdsInFuncMap _mapInlineFunc; // inline functions
    
    // Main builder
    void LdsBuild(bool bExpression);

    // Build statement
    void StatementBuilder(void);
    // Build definitions
    bool DefinitionBuilder(void);
    // Build expression
    void ExpressionBuilder(const LdsFlags &ubFlags);
    
    // Build identifiers
    bool ScopeBuilder(void);
    // Build postfix operations
    bool PostfixBuilder(bool bChained);
    // Build operation
    void OperationBuilder(CLdsToken etFirst);
    // Build loop body
    void BuildLoopBody(void);
    // Build switch's case body
    void BuildSwitchCaseBody(void);
    
  // Compiler
  public:
    CDMap<LdsHash, SLdsCache> _mapScriptCache; // cached scripts by their hash value
    bool _bUseScriptCaching; // cache scripts or not
    
    // General compilation
    ELdsError LdsCompileGeneral(const string &strSource, CActionList &acaActions, const bool &bExpression);
    
    // Compile the script
    ELdsError LdsCompileScript(const string &strScript, CActionList &acaActions);
    // Compile the expression
    ELdsError LdsCompileExpression(const string &strExpression, CActionList &acaActions);

    // Cache a certain script
    void LdsCacheScript(const string &strScript, CActionList &acaScript);

  private:
    // Get the variable
    void CompileGetter(CBuildNode &bn, CActionList &aca);
    // Set the variable
    void CompileSetter(CBuildNode &bn, CActionList &aca);
    // Breaks and continues
    void CompileBreakCont(CActionList &aca, int iStart, int iEnd, int iBreak, int iCont);
    // Shift jumping positions
    void CompileJumpShift(CActionList &aca, int iStart, int iShift);
    // Accessors
    void CompileAccessors(CBuildNode &bn, CActionList &aca, bool bSet);

    // Compile nodes recursively
    void Compile(CBuildNode &bn, CActionList &aca);
    
  // Evaluator
  public:
    // Execute the compiled expression
    CLdsValue LdsExecute(CActionList &acaActions);
    // Evalute the expression
    ELdsError LdsEvaluate(const string &strExpression, CLdsValue &valResult);
    // Evaluate compiled expression
    ELdsError LdsEvaluateCompiled(CActionList &acaActions, CLdsValue &valResult);
    
  // Threads
  public:
    CDList<SLdsHandler> _athhThreadHandlers;
    int _iThreadTickRate; // how many ticks to wait per second (higher = more precise)
    LONG64 _llCurrentTick; // current timer tick (used in I/O)
  
    // Create a new thread
    CLdsThread *ThreadCreate(const CActionList &acaActions, CLdsVarMap &mapArgs);

    // Execute the compiled inline script
    EThreadStatus ScriptExecute(const CActionList &acaActions, CLdsValue *pvalResult,
                                CLdsVarMap &mapArgs = CLdsVarMap(), CLdsInFuncMap *pmapInline = NULL);
    
  public:
    // Constructor
    CLdsScriptEngine(void) :
      // Compatibility
      _pLdsPrintFunction((void (*)(const char *))printf),
      _pLdsErrorFunction((void (*)(const char *))printf),
      _pDestructorFunc(NULL),

      _pLdsLoadScript((bool (*)(const char *, string &))LdsLoadScriptFile),
      _pLdsWrite((void (*)(void *, const void *, const LdsSize &))LdsWriteFile),
      _pLdsRead((void (*)(void *, void *, const LdsSize &))LdsReadFile),
      _pLdsStreamTell((int (*)(void *))LdsFileTell),
      
      // Builder
      _iBuildPos(0),
      _ctBuildLen(0),
      _bBuildBreak(false),
      _bBuildCont(false),

      // Compiler
      _bUseScriptCaching(false),
      
      // Threads
      _iThreadTickRate(64),
      _llCurrentTick(0)
    {
      // Set default functions and variables
      SetDefaultFunctions();
      SetDefaultVariables();
    };
    
    // Destructor
    ~CLdsScriptEngine(void) {
      // call destructor function
      if (_pDestructorFunc != NULL) {
        _pDestructorFunc(this);
      }
      
      // delete remaining threads
      int iThreads = _athhThreadHandlers.Count();

      while (--iThreads >= 0) {
        delete _athhThreadHandlers[iThreads].psthThread;
      }

      _athhThreadHandlers.Clear();
    };
    
    // Thread handling
    void HandleThreads(const LONG64 &llCurrentTick);

    // Get handler index of some thread if it exists
    int ThreadHandlerIndex(CLdsThread *psth);
};
