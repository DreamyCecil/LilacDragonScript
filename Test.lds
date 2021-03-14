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

// script that gets 10 random characters from the string and prints them out

Print("Original string: " + strHello);
Print(""); // empty line

// run it MAX_COUNT times in a loop
var ctRepeat = MAX_COUNT;

while (--ctRepeat >= 0) {
  var strRandom = "";

  var ctRnd = 10;
  while (--ctRnd >= 0) {
    strRandom += strHello[Random() % 13];
  }

  Print((MAX_COUNT-ctRepeat-1) + ": " + strRandom);
}

Print(""); // empty line

// array as a result
strHello = "Goodbye";
return [false, 1.5, strHello];