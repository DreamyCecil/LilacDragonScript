#pragma once

#include "../LdsTypes.h"

struct LDS_API SLdsHandler {
  class CLdsThread *psthThread;
  LONG64 llStartTime; // wait time start (64 bits)
  LONG64 llEndTime; // wait time end (64 bits)
  
  // Contructor
  SLdsHandler(void) : psthThread(NULL), llStartTime(0), llEndTime(0) {};
  
  // Timer constructor
  SLdsHandler(class CLdsThread *plds, LONG64 llStart, LONG64 llEnd) :
    psthThread(plds), llStartTime(llStart), llEndTime(llEnd) {};
    
  // Get waiting time
  LONG64 WaitTime(void) {
    return (llEndTime - llStartTime);
  };
};
