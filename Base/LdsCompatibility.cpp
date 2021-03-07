#include "LdsCompatibility.h"
#include "../LdsScriptEngine.h"

#include <fstream>

// Throw formatted exception
void LdsThrow(const ELdsError &eError, const char *strFormat, ...) {
  va_list arg;
  va_start(arg, strFormat);

  string strError = LdsVPrintF(strFormat, arg);

  // pair error message with the error code
  throw SLdsError(eError, strError);
};

// Standard script loading
bool LdsLoadScriptFile(const char *strFile, string &strScript) {
  std::ifstream strm;
  strm.open(strFile);

  // couldn't open the file
  if (!strm) {
    return false;
  }

  // read until the end
  while (!strm.eof()) {
    string strLine = "";
    std::getline(strm, strLine);

    // save each line into the script string
    strScript += strLine+"\n";
  }
  strm.close();
  
  // loaded successfully
  return true;
};

// Write data into a file
void LdsWriteFile(void *pStream, const void *pData, const LdsSize &iSize) {
  FILE *file = (FILE*)pStream;

  fwrite(pData, iSize, 1, file);
};

// Read data from a file
void LdsReadFile(void *pStream, void *pData, const LdsSize &iSize) {
  FILE *file = (FILE*)pStream;

  fread(pData, iSize, 1, file);
};

// Get current position in a file
int LdsFileTell(void *pStream) {
  FILE *file = (FILE*)pStream;

  return ftell(file);
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

// Set stream functions
void CLdsScriptEngine::LdsStreamFunctions(void *pWrite, void *pRead, void *pTell) {
  _pLdsWrite = (void (*)(void *, const void *, const LdsSize &))pWrite;
  _pLdsRead = (void (*)(void *, void *, const LdsSize &))pRead;
  _pLdsStreamTell = (int (*)(void *))pTell;
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
