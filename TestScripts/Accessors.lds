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

// Script that accesses different values within data structures

// specific data for each operation
var sData = {
  add = 8,
  mul = 10,
  sub = 2,
};

// order of operations
var aOps = ["sub", "add", "mul"];

// resulting number
var iResult = 6;

// print out the expression
Out("(" + iResult + " - " + sData.sub + " + " + sData.add + ") * " + sData.mul + " = ");

for (var i = 0; i < 3; i++) {
  // get operation from the array
  var strOperation = aOps[i];
  
  // get number under the operation name
  var iData = sData[strOperation];
  
  // perform operations
  switch (strOperation) {
    case "add":
      iResult += iData;
      break;
      
    case "mul":
      iResult *= iData;
      break;
      
    case "sub":
      iResult -= iData;
      break;
  }
}

// print out the result
Out(iResult + "\n");