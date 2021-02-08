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