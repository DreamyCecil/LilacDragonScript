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

#include "../LdsBase.h"

// Structure value
struct LDS_API CLdsStruct {
  public:
    // TODO: Each structure would have a unique ID (for example - Serious Sam entity ID) and a function that would be called
    //       everytime a variable in the structure is changed and sends this variable (its position in the map) and a struct ID
    //       That would allow hooking onto certain external objects and change them on the spot.
    //       For example a line like 'entity.m_strName = "this";' would immediately change value in the 'm_strName' variable of this object in memory, not just the structure.

    int iID; // unique ID
    CLdsVarMap mapVars; // structure fields
    bool bStatic; // static struct (cannot add new fields)
  
    // Constructors
    CLdsStruct(void) : iID(-1) {};
    CLdsStruct(const int &iSetID) : iID(iSetID) {};
  
    // Clear the structure
    void Clear(void);
  
    // Structure size
    int Count(void) { return mapVars.Count(); };
  
    // Print one variable
    string Print(int iVar);
  
    // Assignment
    CLdsStruct &operator=(const CLdsStruct &sOther);
  
    // Structure accessor
    CLdsValue &operator[](const string &strVar);
  
    // Find variable index
    int FindIndex(const string &strVar);
};
