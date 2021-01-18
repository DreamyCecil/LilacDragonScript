#pragma once

#include "../LdsBase.h"

// Structure value
struct LDS_API SLdsStruct {
  int iID; // unique ID
  CLdsVarMap mapVars; // structure fields
  bool bStatic; // static struct (cannot add new fields)
  
  // Constructors
  SLdsStruct(void) : iID(-1) {};
  SLdsStruct(const int &iSetID) : iID(iSetID) {};
  
  // Clear the structure
  void Clear(void);
  
  // Structure size
  int Count(void) { return mapVars.Count(); };
  
  // Print one variable
  string Print(int iVar);
  
  // Variable copying
  SLdsStruct &operator=(const CLdsVarMap &map);
  
  // Structure accessor
  SLdsValue &operator[](const string &strVar);
  
  // Find variable index
  int FindIndex(const string &strVar);
};
