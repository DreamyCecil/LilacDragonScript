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

// Script that calculates hash values of strings using different methods

// custom hashing function
function HashCopy(strData, iFactor) {
  var iHash = 0;
  var ctChar = 0;

  for (var iChar = strData[iChar]; iChar != 0; iChar = strData[++ctChar]) {
    // hash it
    iHash = iFactor * iHash + iChar;
  }

  return iHash;
}

// hash a bunch of words
var aWords = ["Lilac", "Dragon", "Script", "That's not a word, that's a sentence."];

for (var i = 0; i < 4; i++) {
  // print the current word
  Out(aWords[i] + ":\n");
  
  // get hashes from the default function, its copy and with a different hashing factor
  var strHashDefault   = PrintHex(Hash(aWords[i]));
  var strHashCustom    = PrintHex(HashCopy(aWords[i], 31));
  var strHashDifferent = PrintHex(HashCopy(aWords[i], 37));
  
  // print them out
  Out(strHashDefault + ", " + strHashCustom + ", " + strHashDifferent + "\n");
}