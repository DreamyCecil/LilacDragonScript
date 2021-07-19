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

#include "StdH.h"

// Escape character prefix
#define ESCAPE_CHAR '\\'

// Script length and parsing positions
static int _ctLen;
static int _iLine;
static int _iLineStart;
static int _iPos;

// Parse until the end
#define UNTIL_END while (_iPos < _ctLen)

// Count one script line
#define COUNT_LINE _iLine++; _iLineStart = _iPos

// Parse line comment
static void ParseLineComment(const string &str) {
  UNTIL_END {
    // line comment end
    if (str[_iPos] == '\r' || str[_iPos] == '\n') {
      break;
    }

    _iPos++;
  }
};

// Parse block comment
static void ParseBlockComment(const string &str) {
  _iPos++;

  UNTIL_END {
    // count lines
    if (str[_iPos] == '\n') {
      COUNT_LINE;

    // block comment end
    } else if (str[_iPos] == '*' && str[_iPos + 1] == '/') {
      _iPos += 2;
      break;
    }

    _iPos++;
  }
};

// Set custom constants from the map
void CLdsScriptEngine::SetParserConstants(CLdsValueMap &mapFrom) {
  // reset the map
  _mapLdsConstants.Clear();
  
  // add custom constants
  _mapLdsConstants.AddFrom(mapFrom, true);
};

// Clamp the value
static inline int Clamp(const int &iNum, const int &iDown, const int &iUp) {
  return (iNum >= iDown ? (iNum <= iUp ? iNum : iUp) : iDown);
};

// Parse the script
void CLdsScriptEngine::ParseScript(string strScript) {
  _aetTokens.Clear();
  string str = strScript;

  _ctLen = str.length();
  
  _iLine = 1;
  _iLineStart = 0;
  _iPos = 0;
  
  UNTIL_END {
    int iStart = _iPos;
    int iPrintPos = (_iLine * 32000 + Clamp(_iPos - _iLineStart, 0, 31999));
    
    char cChar = str[_iPos++];
    char cNextChar = str[_iPos];

    switch (cChar) {
      // skip spaces
      case ' ': case '\t': case '\r': break;
      
      // line break
      case '\n': COUNT_LINE; break;

      // parentheses
      case '(': AddParserToken(LTK_PAR_OPEN, iPrintPos); break;
      case ')': AddParserToken(LTK_PAR_CLOSE, iPrintPos); break;
      // square brackets
      case '[': AddParserToken(LTK_SQ_OPEN, iPrintPos); break;
      case ']': AddParserToken(LTK_SQ_CLOSE, iPrintPos); break;
      // curly brackets
      case '{': AddParserToken(LTK_CUR_OPEN, iPrintPos); break;
      case '}': AddParserToken(LTK_CUR_CLOSE, iPrintPos); break;
      
      // structure accessor
      case '.': AddParserToken(LTK_ACCESS, iPrintPos); break;

      // comma
      case ',': AddParserToken(LTK_COMMA, iPrintPos); break;
      // semicolon
      case ';': AddParserToken(LTK_SEMICOL, iPrintPos); break;
      // colon
      case ':': AddParserToken(LTK_COLON, iPrintPos); break;

      // operators
      case '+':
        switch (cNextChar) {
          case '=': // +=
            _iPos++;
            AddParserToken(LTK_SET, iPrintPos, LOP_ADD);
            break;

          case '+': // ++
            _iPos++;
            AddParserToken(LTK_ADJFIX, iPrintPos, 1);
            break;

          default: AddParserToken(LTK_OPERATOR, iPrintPos, LOP_ADD);
        }
        break;

      case '-':
        switch (cNextChar) {
          case '=': // -=
            _iPos++;
            AddParserToken(LTK_SET, iPrintPos, LOP_SUB);
            break;

          case '-': // --
            _iPos++;
            AddParserToken(LTK_ADJFIX, iPrintPos, -1);
            break;

          default: AddParserToken(LTK_OPERATOR, iPrintPos, LOP_SUB);
        }
        break;

      case '*':
        // *=
        if (cNextChar == '=') {
          _iPos++;
          AddParserToken(LTK_SET, iPrintPos, LOP_MUL);
        } else {
          AddParserToken(LTK_OPERATOR, iPrintPos, LOP_MUL);
        }
        break;

      case '/':
        switch (cNextChar) {
          // /=
          case '=':
            _iPos++;
            AddParserToken(LTK_SET, iPrintPos, LOP_DIV);
            break;

          // comments
          case '/': ParseLineComment(str); break;
          case '*': ParseBlockComment(str); break;

          // division operator
          default: AddParserToken(LTK_OPERATOR, iPrintPos, LOP_DIV);
        }
        break;

      case '%':
        AddParserToken(LTK_OPERATOR, iPrintPos, LOP_FMOD);
        break;

      case '~':
        AddParserToken(LTK_UNARYOP, iPrintPos, UOP_BINVERT);
        break;

      case '$':
        AddParserToken(LTK_UNARYOP, iPrintPos, UOP_STRINGIFY);
        break;

      // bitwise operators
      case '&':
        switch (cNextChar) {
          case '=': // &=
            _iPos++;
            AddParserToken(LTK_SET, iPrintPos, LOP_B_AND);
            break;

          case '&': // &&
            _iPos++;
            AddParserToken(LTK_OPERATOR, iPrintPos, LOP_AND);
            break;

          default: AddParserToken(LTK_OPERATOR, iPrintPos, LOP_B_AND);
        }
        break;

      case '^':
        switch (cNextChar) {
          case '=': // ^=
            _iPos++;
            AddParserToken(LTK_SET, iPrintPos, LOP_B_XOR);
            break;

          case '^': // ^^
            _iPos++;
            AddParserToken(LTK_OPERATOR, iPrintPos, LOP_XOR);
            break;

          default: AddParserToken(LTK_OPERATOR, iPrintPos, LOP_B_XOR);
        }
        break;

      case '|':
        switch (cNextChar) {
          case '=': // |=
            _iPos++;
            AddParserToken(LTK_SET, iPrintPos, LOP_B_OR);
            break;

          case '|': // ||
            _iPos++;
            AddParserToken(LTK_OPERATOR, iPrintPos, LOP_OR);
            break;

          default: AddParserToken(LTK_OPERATOR, iPrintPos, LOP_B_OR);
        }
        break;

      // conditional operators
      case '>':
        switch (cNextChar) {
          case '=': // >=
            _iPos++;
            AddParserToken(LTK_OPERATOR, iPrintPos, LOP_GOE);
            break;

          case '>': // >>
            _iPos++;
            AddParserToken(LTK_OPERATOR, iPrintPos, LOP_SH_R);
            break;

          default: AddParserToken(LTK_OPERATOR, iPrintPos, LOP_GT);
        }
        break;

      case '<':
        switch (cNextChar) {
          case '=': // <=
            _iPos++;
            AddParserToken(LTK_OPERATOR, iPrintPos, LOP_LOE);
            break;

          case '<': // <<
            _iPos++;
            AddParserToken(LTK_OPERATOR, iPrintPos, LOP_SH_L);
            break;

          default: AddParserToken(LTK_OPERATOR, iPrintPos, LOP_LT);
        }
        break;

      case '=':
        // ==
        if (cNextChar == '=') {
          _iPos++;
          AddParserToken(LTK_OPERATOR, iPrintPos, LOP_EQ);
        } else {
          AddParserToken(LTK_SET, iPrintPos, LOP_SET);
        }
        break;

      case '!':
        // !=
        if (cNextChar == '=') {
          _iPos++;
          AddParserToken(LTK_OPERATOR, iPrintPos, LOP_NEQ);
        } else {
          AddParserToken(LTK_UNARYOP, iPrintPos, UOP_INVERT);
        }
        break;

      // string
      case '"': {
        // current character
        bool bEscapeChar = false;
        char cString;

        UNTIL_END {
          // current character
          cString = str[_iPos];

          switch (cString) {
            // line break
            case '\n': COUNT_LINE; break;

            // mark escape characters
            case ESCAPE_CHAR:
              bEscapeChar = !bEscapeChar;
              _iPos++;
              continue;
            
            // string end
            case '"':
              if (!bEscapeChar) {
                goto __FinishString__;
              }
              break;
          }

          bEscapeChar = false;
          _iPos++;
        }

        __FinishString__:

        // didn't parse past the limit
        if (_iPos < _ctLen) {
          string strString = str.substr(iStart + 1, _iPos - iStart - 1);

          // parse escape characters
          std::ostringstream strmEscape;
          bEscapeChar = false;

          for (string::size_type iChar = 0; iChar < strString.length(); iChar++) {
            // current character
            cString = strString[iChar];

            switch (cString) {
              case ESCAPE_CHAR: {
                // has been marked as escape char already, just print it out
                if (bEscapeChar) {
                  strmEscape << ESCAPE_CHAR;
                }

                bEscapeChar = !bEscapeChar;
              } break;

              default: {
                // check other symbols
                if (bEscapeChar) {
                  switch (cString) {
                    case 'n': strmEscape << '\n'; break;
                    case 'r': strmEscape << '\r'; break;
                    case 't': strmEscape << '\t'; break;
                    case '"': strmEscape << '"'; break;
                  
                    // write backslash
                    default: strmEscape << ESCAPE_CHAR;
                  }

                  bEscapeChar = false;
                  break;
                }

                // regular characters
                strmEscape << cString;
              } break;
            }
          }

          AddParserToken(LTK_VAL, iPrintPos, strmEscape.str());
          _iPos++;

        } else {
          LdsThrow(LEP_STRING, "Unclosed string starting at %s", LdsPrintPos(iPrintPos).c_str());
        }
      } break;
      
      // hash symbol
      case '#': {
        AddParserToken(LTK_HASH, iPrintPos);
      } break;

      // constants
      default: {
        if (cChar >= '0' && cChar <= '9') {
          char ubType = 0; // 1 - float, 2 - hex
          bool bHexCheck = (cChar == '0');
          
          // parse through numbers
          UNTIL_END {
            cChar = str[_iPos];

            // change type
            if (cChar == 'x' || cChar == 'X') {
              // can't be a hexadecimal number anymore
              if (!bHexCheck) {
                break;
              }
              
              // register as a hexadecimal number
              ubType = 2;
              _iPos++;

            // encountered a dot
            } else if (cChar == '.') {
              // already changed type
              if (ubType != 0) {
                break;
              }
              
              // register as a float
              ubType = 1;
              _iPos++;
              
            // copy other numbers
            } else if (cChar >= '0' && cChar <= '9') {
              _iPos++;

            // hexadecimal numbers
            } else if (ubType == 2) { 
              if ((cChar >= 'a' && cChar <= 'f') || (cChar >= 'A' && cChar <= 'F')) {
                _iPos++;
              } else {
                break;
              }

            // invalid symbol
            } else {
              break;
            }
            
            // stop checking for hexadecimal numbers
            bHexCheck = false;
          }
          
          // save the number
          string strString = str.substr(iStart, _iPos - iStart);
          
          switch (ubType) {
            case 0: { // index
              int iValue = atoi(strString.c_str());
              AddParserToken(LTK_VAL, iPrintPos, iValue);
            } break;

            case 2: { // hexadecimal index
              int iHexValue = 0;
              SSCANF_FUNC(strString.c_str(), "%x", &iHexValue);
              AddParserToken(LTK_VAL, iPrintPos, iHexValue);
            } break;

            default: { // float
              double dValue = atof(strString.c_str());
              AddParserToken(LTK_VAL, iPrintPos, dValue);
            }
          }
        
        // start identifier names with an underscore or letters
        } else if (cChar == '_'
               || (cChar >= 'a' && cChar <= 'z')
               || (cChar >= 'A' && cChar <= 'Z')) {
          // parse name symbols
          UNTIL_END {
            cChar = str[_iPos];
            
            // allow underscore, letters and numbers
            if (cChar == '_'
            || (cChar >= '0' && cChar <= '9')
            || (cChar >= 'a' && cChar <= 'z')
            || (cChar >= 'A' && cChar <= 'Z')) {
              _iPos++;
              
            // invalid symbol
            } else {
              break;
            }
          }
          
          // copy the name
          string strName = str.substr(iStart, _iPos - iStart);
          
          // operators
          if (strName == "mod") {
            AddParserToken(LTK_OPERATOR, iPrintPos, LOP_FMOD);

          } else if (strName == "div") {
            AddParserToken(LTK_OPERATOR, iPrintPos, LOP_IDIV);

          } else if (strName == "and") {
            AddParserToken(LTK_OPERATOR, iPrintPos, LOP_AND);

          } else if (strName == "or") {
            AddParserToken(LTK_OPERATOR, iPrintPos, LOP_OR);

          // constants
          } else if (strName == "true") {
            AddParserToken(LTK_VAL, iPrintPos, true);
            
          } else if (strName == "false") {
            AddParserToken(LTK_VAL, iPrintPos, false);

          } else if (strName == "pi") {
            AddParserToken(LTK_VAL, iPrintPos, 3.14159265358979323846);

          } else if (strName == "e") {
            AddParserToken(LTK_VAL, iPrintPos, 2.71828182845904523536);

          } else if (strName == "null") {
            AddParserToken(LTK_VAL, iPrintPos, CLdsPtrType());

          // conditions
          } else if (strName == "if") {
            AddParserToken(LTK_IF, iPrintPos);

          } else if (strName == "else") {
            AddParserToken(LTK_ELSE, iPrintPos);

          } else if (strName == "return") {
            AddParserToken(LTK_RETURN, iPrintPos);
          
          // loops
          } else if (strName == "while") {
            AddParserToken(LTK_WHILE, iPrintPos);

          } else if (strName == "do") {
            AddParserToken(LTK_DO, iPrintPos);

          } else if (strName == "for") {
            AddParserToken(LTK_FOR, iPrintPos);

          } else if (strName == "break") {
            AddParserToken(LTK_BREAK, iPrintPos);

          } else if (strName == "continue") {
            AddParserToken(LTK_CONTINUE, iPrintPos);
          
          // switch-case
          } else if (strName == "switch") {
            AddParserToken(LTK_SWITCH, iPrintPos);

          } else if (strName == "case") {
            AddParserToken(LTK_CASE, iPrintPos);

          } else if (strName == "default") {
            AddParserToken(LTK_DEFAULT, iPrintPos);
            
          // other
          } else if (strName == "var") {
            AddParserToken(LTK_VAR, iPrintPos, false);
            
          } else if (strName == "const") {
            AddParserToken(LTK_VAR, iPrintPos, true);
            
          } else if (strName == "static") {
            AddParserToken(LTK_STATIC, iPrintPos);
            
          } else if (strName == "function") {
            AddParserToken(LTK_FUNC, iPrintPos);

          // custom constants
          } else if (_mapLdsConstants.FindKeyIndex(strName) != -1) {
            AddParserToken(LTK_VAL, iPrintPos, _mapLdsConstants[strName]);

          } else {
            AddParserToken(LTK_ID, iPrintPos, strName);
          }
        
        // unknown characters
        } else {
          LdsThrow(LEP_CHAR, "Unexpected character '%c' at %s", cChar, LdsPrintPos(iPrintPos).c_str());
        }
      }
    }
  }

  AddParserToken(LTK_END, _ctLen);
};

// Add expression token
void CLdsScriptEngine::AddParserToken(const ELdsToken &eType, const int &iPos) {
  _aetTokens.Add(CLdsToken(eType, iPos, -1));
};

void CLdsScriptEngine::AddParserToken(const ELdsToken &eType, const int &iPos, const CLdsValue &valValue) {
  _aetTokens.Add(CLdsToken(eType, iPos, valValue, -1));
};
