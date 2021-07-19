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
#include "LdsCompatibility.h"

#include <fstream>

// Calculate simple hash value out of some string
LdsHash GetHash(const string &str) {
  char *strData = (char *)str.c_str();
  LdsHash iHash = 0;

  for (char *pChar = strData; *pChar != '\0'; pChar++) {
    iHash = 31 * iHash + *pChar;
  }

  return iHash;
};

// Throw formatted exception
void LdsThrow(const ELdsError &eError, const char *strFormat, ...) {
  va_list arg;
  va_start(arg, strFormat);

  string strError = LdsVPrintF(strFormat, arg);
  va_end(arg);

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

// Add new value type
void CLdsScriptEngine::AddValueType(const ILdsValueBase &val) {
  _ldsValueTypes.Add(val.MakeCopy());
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
  _pLdsWrite = (CLdsWriteFunc)pWrite;
  _pLdsRead = (CLdsReadFunc)pRead;
  _pLdsStreamTell = (int (*)(void *))pTell;
};

// Print out formatted string
void CLdsScriptEngine::LdsOut(const char *strFormat, ...) {
  va_list arg;
  va_start(arg, strFormat);

  string strOut = LdsVPrintF(strFormat, arg);
  va_end(arg);

  _pLdsPrintFunction(strOut.c_str());
};

// Print out an error
void CLdsScriptEngine::LdsErrorOut(const char *strFormat, ...) {
  va_list arg;
  va_start(arg, strFormat);

  string strOut = LdsVPrintF(strFormat, arg);
  va_end(arg);

  _pLdsErrorFunction(strOut.c_str());
};
