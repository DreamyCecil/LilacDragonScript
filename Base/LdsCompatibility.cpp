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

// Print out formatted string
void CLdsScriptEngine::LdsOut(const char *strFormat, ...) {
  va_list arg;
  va_start(arg, strFormat);

  string strOut = LdsVPrintF(strFormat, arg);

  // no output function
  if (_pLdsPrintFunction == NULL) {
    printf(strOut.c_str());
    return;
  }

  _pLdsPrintFunction(strOut.c_str());
};

// Print out an error
void CLdsScriptEngine::LdsErrorOut(const char *strFormat, ...) {
  va_list arg;
  va_start(arg, strFormat);

  string strOut = LdsVPrintF(strFormat, arg);

  // check the error function
  if (_pLdsErrorFunction == NULL) {
    // check the print function
    if (_pLdsPrintFunction == NULL) {
      printf(strOut.c_str());
      return;
    }

    _pLdsPrintFunction(strOut.c_str());
    return;
  }

  _pLdsErrorFunction(strOut.c_str());
};
