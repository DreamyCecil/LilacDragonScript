#include "LdsCompatibility.h"
#include "../LdsScriptEngine.h"

// Throw formatted exception
void LdsThrow(const ELdsError &eError, const char *strFormat, ...) {
  va_list arg;
  va_start(arg, strFormat);

  string strError = LdsVPrintF(strFormat, arg);

  // pair error message with the error code
  throw SLdsError(eError, strError);
};

// Set output printing functions
void CLdsScriptEngine::LdsOutputFunctions(void *pPrint, void *pError) {
  // reset to standard printing function
  if (pPrint == NULL) {
    pPrint = printf;
  }
  _pLdsPrintFunction = (void (*)(const char *))pPrint;

  // copy print function if no error function
  if (pError == NULL) {
    pError = pPrint;
  }
  _pLdsErrorFunction = (void (*)(const char *))pError;
};

// Print out formatted string
void CLdsScriptEngine::LdsOut(const char *strFormat, ...) {
  va_list arg;
  va_start(arg, strFormat);

  string strOut = LdsVPrintF(strFormat, arg);
  _pLdsPrintFunction(strOut.c_str());
};

// Print out an error
void CLdsScriptEngine::LdsErrorOut(const char *strFormat, ...) {
  va_list arg;
  va_start(arg, strFormat);

  string strOut = LdsVPrintF(strFormat, arg);
  _pLdsErrorFunction(strOut.c_str());
};
