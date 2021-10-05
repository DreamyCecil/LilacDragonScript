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

// Script that calculates and prints two waves using unary math operators

var a = [0, 1];
const iLength = 20;

for (var iAngle = 0; iAngle <= 360; iAngle += 10) {
  var rad1 = (iAngle - 90) * pi/180;
  var rad2 = iAngle * pi/180;
  
  a[0] = sin rad1 + 1;
  a[1] = cos rad2 + 1;
  
  var str = "";
  
  for (var i = 0; i <= iLength; i++) {
    if (abs(a[0] * iLength/2 - i) < 1
     || abs(a[1] * iLength/2 - i) < 1) {
      str += "#";
    } else {
      str += " ";
    }
  }
  
  Out(str + "\n");
}