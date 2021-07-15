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

// Script that prints out prime numbers until 50

for (var ct = 2; ct <= 50; ct++)
{
  // print it out if it's a prime number
  var bPrime = IsPrime(ct);
  
  if (bPrime) {
    Out(ct + ", ");
  }
}

// and so on...
return Out("...\n");

// check if the number is prime
function IsPrime(iNumber) {
  // check if the current number is divisible by others
  for (var iLower = 2; iLower < iNumber; iLower++)
  {
    // not a prime number
    if (iNumber % iLower == 0) {
      return false;
    }
  }
  
  // prime number
  return true;
};