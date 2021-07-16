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

// Secure scanning function
#if defined(_MSC_VER) && _MSC_VER >= 1700
  #define SSCANF_FUNC sscanf_s
#else
  #define SSCANF_FUNC sscanf
#endif

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

  _ctLen = strlen(str.c_str());
  
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
      case '(': AddLdsToken(LTK_PAR_OPEN, iPrintPos); break;
      case ')': AddLdsToken(LTK_PAR_CLOSE, iPrintPos); break;
      // square brackets
      case '[': AddLdsToken(LTK_SQ_OPEN, iPrintPos); break;
      case ']': AddLdsToken(LTK_SQ_CLOSE, iPrintPos); break;
      // curly brackets
      case '{': AddLdsToken(LTK_CUR_OPEN, iPrintPos); break;
      case '}': AddLdsToken(LTK_CUR_CLOSE, iPrintPos); break;
      
      // structure accessor
      case '.': AddLdsToken(LTK_ACCESS, iPrintPos); break;

      // comma
      case ',': AddLdsToken(LTK_COMMA, iPrintPos); break;
      // semicolon
      case ';': AddLdsToken(LTK_SEMICOL, iPrintPos); break;
      // colon
      case ':': AddLdsToken(LTK_COLON, iPrintPos); break;

      // operators
      case '+':
        switch (cNextChar) {
          case '=': // +=
            _iPos++;
            AddLdsToken(LTK_SET, iPrintPos, LOP_ADD);
            break;

          case '+': // ++
            _iPos++;
            AddLdsToken(LTK_ADJFIX, iPrintPos, 1);
            break;

          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_ADD);
        }
        break;

      case '-':
        switch (cNextChar) {
          case '=': // -=
            _iPos++;
            AddLdsToken(LTK_SET, iPrintPos, LOP_SUB);
            break;

          case '-': // --
            _iPos++;
            AddLdsToken(LTK_ADJFIX, iPrintPos, -1);
            break;

          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_SUB);
        }
        break;

      case '*':
        // *=
        if (cNextChar == '=') {
          _iPos++;
          AddLdsToken(LTK_SET, iPrintPos, LOP_MUL);
        } else {
          AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_MUL);
        }
        break;

      case '/':
        switch (cNextChar) {
          // /=
          case '=':
            _iPos++;
            AddLdsToken(LTK_SET, iPrintPos, LOP_DIV);
            break;

          // comments
          case '/': ParseLineComment(str); break;
          case '*': ParseBlockComment(str); break;

          // division operator
          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_DIV);
        }
        break;

      case '%':
        AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_FMOD);
        break;

      case '~':
        AddLdsToken(LTK_UNARYOP, iPrintPos, UOP_BINVERT);
        break;

      // bitwise operators
      case '&':
        switch (cNextChar) {
          case '=': // &=
            _iPos++;
            AddLdsToken(LTK_SET, iPrintPos, LOP_B_AND);
            break;

          case '&': // &&
            _iPos++;
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_AND);
            break;

          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_B_AND);
        }
        break;

      case '^':
        switch (cNextChar) {
          case '=': // ^=
            _iPos++;
            AddLdsToken(LTK_SET, iPrintPos, LOP_B_XOR);
            break;

          case '^': // ^^
            _iPos++;
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_XOR);
            break;

          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_B_XOR);
        }
        break;

      case '|':
        switch (cNextChar) {
          case '=': // |=
            _iPos++;
            AddLdsToken(LTK_SET, iPrintPos, LOP_B_OR);
            break;

          case '|': // ||
            _iPos++;
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_OR);
            break;

          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_B_OR);
        }
        break;

      // conditional operators
      case '>':
        switch (cNextChar) {
          case '=': // >=
            _iPos++;
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_GOE);
            break;

          case '>': // >>
            _iPos++;
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_SH_R);
            break;

          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_GT);
        }
        break;

      case '<':
        switch (cNextChar) {
          case '=': // <=
            _iPos++;
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_LOE);
            break;

          case '<': // <<
            _iPos++;
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_SH_L);
            break;

          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_LT);
        }
        break;

      case '=':
        // ==
        if (cNextChar == '=') {
          _iPos++;
          AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_EQ);
        } else {
          AddLdsToken(LTK_SET, iPrintPos, LOP_SET);
        }
        break;

      case '!':
        // !=
        if (cNextChar == '=') {
          _iPos++;
          AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_NEQ);
        } else {
          AddLdsToken(LTK_UNARYOP, iPrintPos, UOP_INVERT);
        }
        break;

      // string
      case '"': {
        // resulting string
        string strString = "";
        
        // current character
        bool bEscapeChar = false;
        char cString;

        UNTIL_END {
          // current character
          cString = str[_iPos];

          bool bContinueString = false;

          switch (cString) {
            // line break
            case '\n': COUNT_LINE; break;

            // mark escape characters
            case ESCAPE_CHAR:
              bEscapeChar = !bEscapeChar;
              _iPos++;
            
              // add character to the string
              strString += cString;
              continue;
            
            // string end
            case '"': {
              // or actually not
              if (bEscapeChar) {
                break;
              }

              // store current position
              int iLastPos = _iPos;
              int iLastLine = _iLine;
              int iLastLineStart = _iLineStart;

              bool bStringAddition = false;
            
              // search for the next string if there is one
              UNTIL_END {
                // start with the next character
                _iPos++;

                cChar = str[_iPos];
                cNextChar = str[_iPos + 1];

                switch (cChar) {
                  // skip spaces
                  case ' ': case '\t': case '\r': break;

                  // line break
                  case '\n': COUNT_LINE; break;

                  // comments
                  case '/': {
                    switch (cNextChar) {
                      case '/': ParseLineComment(str); break;
                      case '*': ParseBlockComment(str); break;
                    }
                  } break;

                  // addition of another string
                  case '+': {
                    // can't have two operators in a row
                    if (bStringAddition) {
                      goto __AfterNextString__;
                    }

                    bStringAddition = true;
                  } break;

                  // start of a new string
                  case '"': {
                    bContinueString = true;
                  } goto __AfterNextString__;

                  // no next string if any other character
                  default:
                    goto __AfterNextString__;
                }
              }

              __AfterNextString__:

              // if not followed by another string
              if (!bContinueString) {
                // restore the position
                _iPos = iLastPos;
                _iLine = iLastLine;
                _iLineStart = iLastLineStart;

                // finish parsing the string
                goto __FinishString__;
              }
            } break;
          }

          bEscapeChar = false;
          _iPos++;

          // add character to the string
          if (!bContinueString) {
            strString += cString;
          }
        }

        __FinishString__:

        // didn't parse past the limit
        if (_iPos < _ctLen) {
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

          AddLdsToken(LTK_VAL, iPrintPos, strmEscape.str());
          _iPos++;

        } else {
          LdsThrow(LEP_STRING, "Unclosed string starting at %s", LdsPrintPos(iPrintPos).c_str());
        }
      } break;
      
      // thread directive
      case '#': {
        cChar = cNextChar;
        
        // start directive names with an underscore or letters
        if (cChar == '_'
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
          
          // copy the name (excluding '#')
          string strDir = str.substr(iStart+1, _iPos - iStart - 1);
          
          // determine directive type by its name
          int iDirType = -1;
          
          if (strDir == "context") {
            iDirType = THD_DEBUGCONTEXT;
            
          } else {
            LdsThrow(LEP_DIR, "Unknown directive '%s' at %s", strDir.c_str(), LdsPrintPos(iPrintPos).c_str());
          }
          
          AddLdsToken(LTK_DIR, iPrintPos, iDirType);
          
        } else {
          LdsThrow(LEP_DIRNAME, "Expected a directive name at %s", LdsPrintPos(iPrintPos).c_str());
        }
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
              AddLdsToken(LTK_VAL, iPrintPos, iValue);
            } break;

            case 2: { // hexadecimal index
              int iHexValue = 0;
              SSCANF_FUNC(strString.c_str(), "%x", &iHexValue);
              AddLdsToken(LTK_VAL, iPrintPos, iHexValue);
            } break;

            default: { // float
              double dValue = atof(strString.c_str());
              AddLdsToken(LTK_VAL, iPrintPos, dValue);
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
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_FMOD);

          } else if (strName == "div") {
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_IDIV);

          } else if (strName == "and") {
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_AND);

          } else if (strName == "or") {
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_OR);

          // constants
          } else if (strName == "true") {
            AddLdsToken(LTK_VAL, iPrintPos, true);
            
          } else if (strName == "false") {
            AddLdsToken(LTK_VAL, iPrintPos, false);

          } else if (strName == "pi") {
            AddLdsToken(LTK_VAL, iPrintPos, 3.14159265358979323846);

          } else if (strName == "e") {
            AddLdsToken(LTK_VAL, iPrintPos, 2.71828182845904523536);

          // conditions
          } else if (strName == "if") {
            AddLdsToken(LTK_IF, iPrintPos);

          } else if (strName == "else") {
            AddLdsToken(LTK_ELSE, iPrintPos);

          } else if (strName == "return") {
            AddLdsToken(LTK_RETURN, iPrintPos);
          
          // loops
          } else if (strName == "while") {
            AddLdsToken(LTK_WHILE, iPrintPos);

          } else if (strName == "do") {
            AddLdsToken(LTK_DO, iPrintPos);

          } else if (strName == "for") {
            AddLdsToken(LTK_FOR, iPrintPos);

          } else if (strName == "break") {
            AddLdsToken(LTK_BREAK, iPrintPos);

          } else if (strName == "continue") {
            AddLdsToken(LTK_CONTINUE, iPrintPos);
          
          // switch-case
          } else if (strName == "switch") {
            AddLdsToken(LTK_SWITCH, iPrintPos);

          } else if (strName == "case") {
            AddLdsToken(LTK_CASE, iPrintPos);

          } else if (strName == "default") {
            AddLdsToken(LTK_DEFAULT, iPrintPos);
            
          // other
          } else if (strName == "var") {
            AddLdsToken(LTK_VAR, iPrintPos, false);
            
          } else if (strName == "const") {
            AddLdsToken(LTK_VAR, iPrintPos, true);
            
          } else if (strName == "static") {
            AddLdsToken(LTK_STATIC, iPrintPos);
            
          } else if (strName == "function") {
            AddLdsToken(LTK_FUNC, iPrintPos);

          // custom constants
          } else if (_mapLdsConstants.FindKeyIndex(strName) != -1) {
            AddLdsToken(LTK_VAL, iPrintPos, _mapLdsConstants[strName]);

          } else {
            AddLdsToken(LTK_ID, iPrintPos, strName);
          }
        
        // unknown characters
        } else {
          LdsThrow(LEP_CHAR, "Unexpected character '%c' at %s", cChar, LdsPrintPos(iPrintPos).c_str());
        }
      }
    }
  }

  AddLdsToken(LTK_END, _ctLen);
};

// Add expression token
void CLdsScriptEngine::AddLdsToken(const ELdsToken &eType, const int &iPos) {
  _aetTokens.Add(CLdsToken(eType, iPos, -1));
};

void CLdsScriptEngine::AddLdsToken(const ELdsToken &eType, const int &iPos, const int &iValue) {
  _aetTokens.Add(CLdsToken(eType, iPos, iValue, -1));
};

void CLdsScriptEngine::AddLdsToken(const ELdsToken &eType, const int &iPos, const double &dValue) {
  _aetTokens.Add(CLdsToken(eType, iPos, dValue, -1));
};

void CLdsScriptEngine::AddLdsToken(const ELdsToken &eType, const int &iPos, const string &strValue) {
  _aetTokens.Add(CLdsToken(eType, iPos, strValue, -1));
};

void CLdsScriptEngine::AddLdsToken(const ELdsToken &eType, const int &iPos, const CLdsValue &valValue) {
  _aetTokens.Add(CLdsToken(eType, iPos, valValue, -1));
};
