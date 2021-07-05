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

#include "LdsFormatting.h"

// Resize raw string
void LdsResizeString(char **pMem, int ctSize) {
  char *pNew = new char[ctSize];

  memcpy(pNew, &pMem, ctSize);
  delete[] &pMem;

  *pMem = pNew;
};

// Format a string
string LdsPrintF(const char *strFormat, ...) {
  va_list arg;
  va_start(arg, strFormat);

  return LdsVPrintF(strFormat, arg);
};

// Format some string using a list of arguments
string LdsVPrintF(const char *strFormat, va_list arg) {
  // allocate new buffer
  int ctBufferSize = 256;
  char *pchBuffer = new char[ctBufferSize];

  // repeat
  int iLen;
  while (true) {
    // print to the buffer
    iLen = _vsnprintf(pchBuffer, ctBufferSize, strFormat, arg);

    // stop if printed ok
    if (iLen != -1) {
      break;
    }

    // increase the buffer size
    ctBufferSize += 256;
    LdsResizeString(&pchBuffer, ctBufferSize);
  }

  string strPrint = pchBuffer;
  delete[] pchBuffer;

  return strPrint;
};

// Convert float number into a string
string LdsFloatStr(const float &fNum) {
  string str = LdsPrintF("%f", fNum);

  // check for a dot
  int iDot = str.find('.');
  if (iDot == string::npos) {
    return str;
  }

  int ctLen = strlen(str.c_str())-1;

  // remove zeros from the end
  while (str.substr(ctLen, 1) == "0") {
    str = str.substr(0, ctLen);
    ctLen--;

    // check for a dot
    if (str.substr(ctLen, 1) == ".") {
      return str.substr(0, ctLen);
    }
  }
  return str;
};

// Print script position
string LdsPrintPos(const int &iPos) {
  int iCol = iPos % 32000; // column
  int iLine = ((iPos - iCol) / 32000); // line

  // too many columns
  string strCol = "..";

  if (iCol < 31999) {
    strCol = LdsPrintF("%d", iCol+1);
  }

  return LdsPrintF("line %d, col %s", iLine, strCol.c_str());
};
