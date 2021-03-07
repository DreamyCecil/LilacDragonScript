#include "LdsScriptEngine.h"

// Write program
void CLdsScriptEngine::LdsWriteProgram(void *pStream, CActionList &acaProgram) {
  int ctActions = acaProgram.Count();
  _pLdsWrite(pStream, &ctActions, sizeof(int));

  // for each action
  for (int iAction = 0; iAction < ctActions; iAction++) {
    LdsWriteAction(pStream, acaProgram[iAction]);
  }
};

// Read program
void CLdsScriptEngine::LdsReadProgram(void *pStream, CActionList &acaProgram) {
  int ctActions = 0;
  _pLdsRead(pStream, &ctActions, sizeof(int));

  // for each action
  for (int iAction = 0; iAction < ctActions; iAction++) {
    CCompAction caAction;
    LdsReadAction(pStream, caAction);

    acaProgram.Add(caAction);
  }
};

// Write action
void CLdsScriptEngine::LdsWriteAction(void *pStream, CCompAction &caAction) {
  // write type and position
  _pLdsWrite(pStream, &caAction.lt_eType, sizeof(int));
  _pLdsWrite(pStream, &caAction.lt_iPos, sizeof(int));

  // write certain action
  switch (caAction.lt_eType) {
    // all data
    case LCA_VAL: case LCA_BIN: case LCA_VAR:
    case LCA_CALL: case LCA_INLINE:
    case LCA_SET: case LCA_GET: case LCA_DIR:
      LdsWriteValue(pStream, caAction.lt_valValue);
      _pLdsWrite(pStream, &caAction.lt_iArg, sizeof(int));
      break;

    // only value
    case LCA_UN:
      LdsWriteValue(pStream, caAction.lt_valValue);
      break;

    // inline function
    case LCA_FUNC:
      LdsWriteValue(pStream, caAction.lt_valValue);
      LdsWriteInlineFunc(pStream, caAction.ca_inFunc);
      break;

    // only argument
    case LCA_JUMP: case LCA_JUMPUNLESS: case LCA_JUMPIF:
    case LCA_AND: case LCA_OR: case LCA_SWITCH:
      _pLdsWrite(pStream, &caAction.lt_iArg, sizeof(int));
      break;

    // no data
    case LCA_SET_ACCESS:
    case LCA_RETURN: case LCA_DISCARD: case LCA_DUP:
      break;

    default: LdsThrow(LER_WRITE, "Cannot write action %s at %d!", _astrActionNames[caAction.lt_eType], _pLdsStreamTell(pStream));
  }
};

// Read action
void CLdsScriptEngine::LdsReadAction(void *pStream, CCompAction &caAction) {
  // read type and position
  _pLdsRead(pStream, &caAction.lt_eType, sizeof(int));
  _pLdsRead(pStream, &caAction.lt_iPos, sizeof(int));

  // read certain action
  switch (caAction.lt_eType) {
    // all data
    case LCA_VAL: case LCA_BIN: case LCA_VAR:
    case LCA_CALL: case LCA_INLINE:
    case LCA_SET: case LCA_GET: case LCA_DIR:
      LdsReadValue(pStream, caAction.lt_valValue);
      _pLdsRead(pStream, &caAction.lt_iArg, sizeof(int));
      break;

    // only value
    case LCA_UN:
      LdsReadValue(pStream, caAction.lt_valValue);
      break;

    // inline function
    case LCA_FUNC:
      LdsReadValue(pStream, caAction.lt_valValue);
      LdsReadInlineFunc(pStream, caAction.ca_inFunc);
      break;

    // only argument
    case LCA_JUMP: case LCA_JUMPUNLESS: case LCA_JUMPIF:
    case LCA_AND: case LCA_OR: case LCA_SWITCH:
      _pLdsRead(pStream, &caAction.lt_iArg, sizeof(int));
      break;

    // no data
    case LCA_SET_ACCESS:
    case LCA_RETURN: case LCA_DISCARD: case LCA_DUP:
      break;

    default: LdsThrow(LER_READ, "Cannot read action %s at %d!", _astrActionNames[caAction.lt_eType], _pLdsStreamTell(pStream));
  }
};

// Write inline function
void CLdsScriptEngine::LdsWriteInlineFunc(void *pStream, SLdsInlineFunc &inFunc) {
  // write arguments
  int ctArgs = inFunc.in_astrArgs.Count();
  _pLdsWrite(pStream, &ctArgs, sizeof(ctArgs));

  for (int iArg = 0; iArg < ctArgs; iArg++) {
    // write argument name as a string value
    LdsWriteValue(pStream, CLdsValue(inFunc.in_astrArgs[iArg]));
  }

  // write the function
  LdsWriteProgram(pStream, inFunc.in_acaFunc);

  // write inline functions
  int ctFunc = inFunc.in_mapInlineFunc.Count();
  _pLdsWrite(pStream, &ctFunc, sizeof(ctFunc));

  for (int iFunc = 0; iFunc < ctFunc; iFunc++) {
    // write key as a string value
    LdsWriteValue(pStream, CLdsValue(inFunc.in_mapInlineFunc.GetKey(iFunc)));

    LdsWriteInlineFunc(pStream, inFunc.in_mapInlineFunc.GetValue(iFunc));
  }
};

// Read inline function
void CLdsScriptEngine::LdsReadInlineFunc(void *pStream, SLdsInlineFunc &inFunc) {
  // read arguments
  int ctArgs = 0;
  _pLdsRead(pStream, &ctArgs, sizeof(ctArgs));

  for (int iArg = 0; iArg < ctArgs; iArg++) {
    // read argument name
    CLdsValue valArg;
    LdsReadValue(pStream, valArg);

    inFunc.in_astrArgs.Add(valArg.strValue);
  }

  // read the function
  LdsReadProgram(pStream, inFunc.in_acaFunc);

  // read inline functions
  int ctFunc = 0;
  _pLdsRead(pStream, &ctFunc, sizeof(ctFunc));

  for (int iFunc = 0; iFunc < ctFunc; iFunc++) {
    // read key
    CLdsValue valFunc;
    LdsReadValue(pStream, valFunc);

    SLdsInlineFunc inInline;
    LdsReadInlineFunc(pStream, inInline);

    inFunc.in_mapInlineFunc.Add(valFunc.strValue, inInline);
  }
};

// Write value
void CLdsScriptEngine::LdsWriteValue(void *pStream, CLdsValue &val) {
  char iType = val.eType;
  _pLdsWrite(pStream, &iType, sizeof(char));

  switch (iType) {
    // integer
    case EVT_INDEX: {
      _pLdsWrite(pStream, &val.iValue, sizeof(int));
    } break;

    // float number
    case EVT_FLOAT: {
      _pLdsWrite(pStream, &val.fValue, sizeof(float));
    } break;

    // string
    case EVT_STRING: {
      string &strWrite = val.strValue;
      const int ctLen = strWrite.size();

      // write string length
      _pLdsWrite(pStream, &ctLen, sizeof(int));

      // write the string
      _pLdsWrite(pStream, strWrite.c_str(), sizeof(char) * ctLen);
    } break;

    // array
    case EVT_ARRAY: {
      CLdsArray &aArray = val.aArray;
      const int ctArray = aArray.Count();

      // write array count
      _pLdsWrite(pStream, &ctArray, sizeof(int));

      // write each individual array value
      for (int i = 0; i < ctArray; i++) {
        LdsWriteValue(pStream, aArray[i]);
      }
    } break;

    // structure
    case EVT_STRUCT: {
      CLdsStruct &sStruct = val.sStruct;
      const int ctStruct = sStruct.Count();

      // write structure ID and if it's static
      _pLdsWrite(pStream, &sStruct.iID, sizeof(int));
      _pLdsWrite(pStream, &sStruct.bStatic, sizeof(bool));

      // write amount of keys
      _pLdsWrite(pStream, &ctStruct, sizeof(int));

      // write each individual key-value pair
      for (int i = 0; i < ctStruct; i++) {
        // write key as a string value
        CLdsValue valKey = sStruct.mapVars.GetKey(i);
        LdsWriteValue(pStream, valKey);
        
        // write variable const type
        SLdsVar &var = sStruct.mapVars.GetValue(i);
        _pLdsWrite(pStream, &var.var_bConst, sizeof(char));

        // write variable value
        LdsWriteValue(pStream, var.var_valValue);
      }
    } break;

    default: LdsThrow(LER_WRITE, "Cannot write value type %d at %d!", iType, _pLdsStreamTell(pStream));
  }
};

// Read value
void CLdsScriptEngine::LdsReadValue(void *pStream, CLdsValue &val) {
  char iType = -1;
  _pLdsRead(pStream, &iType, sizeof(char));

  switch (iType) {
    // integer
    case EVT_INDEX: {
      int iInteger = -1;
      _pLdsRead(pStream, &iInteger, sizeof(int));

      val = iInteger;
    } break;

    // float number
    case EVT_FLOAT: {
      float fNumber = 0.0f;
      _pLdsRead(pStream, &fNumber, sizeof(float));

      val = fNumber;
    } break;

    // string
    case EVT_STRING: {
      // read string length
      int ctLen = 0;
      _pLdsRead(pStream, &ctLen, sizeof(int));

      // make a new string
      char *str = new char[ctLen+1];
      str[ctLen] = 0;
      
      // read the string
      _pLdsRead(pStream, str, sizeof(char) * ctLen);

      val = string(str);

      // discard the string
      delete[] str;
    } break;

    // array
    case EVT_ARRAY: {
      // read array count
      int ctArray = 0;
      _pLdsRead(pStream, &ctArray, sizeof(int));

      // create an empty array
      val = CLdsValue(ctArray, (int)0);

      // read values into the array
      for (int i = 0; i < ctArray; i++) {
        LdsReadValue(pStream, val.aArray[i]);
      }
    } break;

    // structure
    case EVT_STRUCT: {
      // read structure ID and if it's static
      int iID = -1;
      bool bStatic = false;
      _pLdsRead(pStream, &iID, sizeof(int));
      _pLdsRead(pStream, &bStatic, sizeof(bool));

      // read amount of keys
      int ctStruct = 0;
      _pLdsRead(pStream, &ctStruct, sizeof(int));

      // create a variable map
      CLdsVarMap mapVars;

      // read each individual key-value pair
      for (int i = 0; i < ctStruct; i++) {
        // read key string
        CLdsValue valKey;
        LdsReadValue(pStream, valKey);
        
        // read variable const type
        SLdsVar var;
        _pLdsRead(pStream, &var.var_bConst, sizeof(char));

        // read variable value
        LdsReadValue(pStream, var.var_valValue);

        // add one key-value pair
        mapVars.Add(valKey.strValue, var);
      }

      // create a structure
      val = CLdsValue(iID, mapVars, bStatic);
    } break;

    default: LdsThrow(LER_READ, "Cannot read value type %d at %d!", iType, _pLdsStreamTell(pStream));
  }
};
