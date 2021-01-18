#include "LdsFormatting.h"

// Resize raw string
char *LdsResizeString(void *pMem, int ctSize) {
  char *pNew = new char[ctSize];

  memcpy(pNew, pMem, ctSize);
  delete[] pMem;

  return pNew;
};

// Format a string
string LdsPrintF(const char *strFormat, ...) {
  va_list arg;
  va_start(arg, strFormat);

  return LdsVPrintF(strFormat, arg);
};

// Format some string using a list of arguments
string LdsVPrintF(const char *strFormat, va_list arg) {
  static int _ctBufferSize = 0;
  static char *_pchBuffer = NULL;

  // allocate if buffer wasn't allocated yet
  if (_ctBufferSize == 0) {
    _ctBufferSize = 256;
    _pchBuffer = new char[_ctBufferSize];
  }

  // repeat
  int iLen;
  while (true) {
    // print to the buffer
    iLen = _vsnprintf(_pchBuffer, _ctBufferSize, strFormat, arg);

    // stop if printed ok
    if (iLen != -1) {
      break;
    }

    // increase the buffer size
    _ctBufferSize += 256;
    _pchBuffer = LdsResizeString(_pchBuffer, _ctBufferSize);
  }

  string strPrint = _pchBuffer;
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
