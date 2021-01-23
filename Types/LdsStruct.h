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
