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

#include "LdsVar.h"

// Object change callback function
typedef void (*CLdsObjectCallback)(class CLdsObject *poObject, const int &iVariable);

// Object value
class LDS_API CLdsObject {
  public:
    // TODO: Each object would have a unique ID (for example - Serious Sam entity ID) and a function that would be called
    //       everytime a property in the object is changed and sends this property (its position in the list) and the object ID
    //       That would allow hooking onto certain external objects and change them on the spot.
    //       For example a line like 'entity.m_strName = "this";' would immediately change value in the 'm_strName' variable of this entity in memory, not just the object.

    int iID; // unique ID
    CLdsVars aFields; // object property fields
    bool bStatic; // static object (cannot add new properties)

    CLdsObjectCallback pCallback; // callback function
  
  public:
    // Constructors
    CLdsObject(void);
    CLdsObject(const int &iSetID);
  
    // Clear the object
    void Clear(void);
  
    // Object size
    inline int Count(void) const {
      return aFields.Count();
    };
  
    // Print one property
    string Print(int iVar);
  
    // Assignment
    CLdsObject &operator=(const CLdsObject &sOther);
  
    // Property accessor
    CLdsValue &operator[](const string &strVar);
  
    // Find property index
    int FindIndex(const string &strVar);
};
