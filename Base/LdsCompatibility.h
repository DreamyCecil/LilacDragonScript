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

#include "LdsBase.h"

// Secure string functions
#if defined(_MSC_VER) && _MSC_VER >= 1700
  #define SPRINTF_FUNC sprintf_s
  #define SSCANF_FUNC  sscanf_s
#else
  #define SPRINTF_FUNC sprintf
  #define SSCANF_FUNC  sscanf
#endif

// XOR check
#define XOR_CHECK(_Cond1, _Cond2) ((int(_Cond1) ^ int(_Cond2)) != 0)

// Standard script loading
LDS_API bool LdsLoadScriptFile(const char *strFile, string &strScript);

// Write data into a file
LDS_API void LdsWriteFile(void *pStream, const void *pData, const LdsSize &iSize);
// Read data from a file
LDS_API void LdsReadFile(void *pStream, void *pData, const LdsSize &iSize);
// Get current position in a file
LDS_API int LdsFileTell(void *pStream);
