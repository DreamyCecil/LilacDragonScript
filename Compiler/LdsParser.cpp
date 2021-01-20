#include "../LdsScriptEngine.h"
#include <math.h>

// Clamp the value
inline float Clamp(const float &fNum, const float &fDown, const float &fUp) {
  return (fNum >= fDown ? (fNum <= fUp ? fNum : fUp) : fDown);
};

// Parse the script
void CLdsScriptEngine::ParseScript(string strScript) {
  _aetTokens.Clear();
  string str = strScript;

  int ctLen = strlen(str.c_str());
  
  int iLine = 1;
  int iLineStart = 0;
  int iPos = 0;
  
  // until the end
  while (iPos < ctLen) {
    int iStart = iPos;
    int iPrintPos = (iLine * 32000 + Clamp(iPos - iLineStart, 0, 31999));
    
    int iChar = str[iPos++];

    switch (iChar) {
      // skip spaces
      case ' ': case '\t': case '\r': break;
      
      // next line
      case '\n':
        iLine++;
        iLineStart = iPos;
        break;

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
        switch (str[iPos]) {
          case '=': // +=
            iPos++;
            AddLdsToken(LTK_SET, iPrintPos, LOP_ADD);
            break;

          case '+': // ++
            iPos++;
            AddLdsToken(LTK_ADJFIX, iPrintPos, 1);
            break;

          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_ADD);
        }
        break;

      case '-':
        switch (str[iPos]) {
          case '=': // -=
            iPos++;
            AddLdsToken(LTK_SET, iPrintPos, LOP_SUB);
            break;

          case '-': // --
            iPos++;
            AddLdsToken(LTK_ADJFIX, iPrintPos, -1);
            break;

          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_SUB);
        }
        break;

      case '*':
        // *=
        if (str[iPos] == '=') {
          iPos++;
          AddLdsToken(LTK_SET, iPrintPos, LOP_MUL);
        } else {
          AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_MUL);
        }
        break;

      case '/':
        switch (str[iPos]) {
          // /=
          case '=':
            iPos++;
            AddLdsToken(LTK_SET, iPrintPos, LOP_DIV);
            break;

          // line comment
          case '/':
            while (iPos < ctLen) {
              iChar = str[iPos];
            
              if (iChar == '\r' || iChar == '\n') {
                break;
              }
            
              iPos++;
            }
            break;
          
          // block comment
          case '*':
            iPos++;
          
            while (iPos < ctLen) {
              // count lines
              if (str[iPos] == '\n') {
                iLine++;
                iLineStart = iPos;
              }
              
              if (str[iPos] == '*' && str[iPos+1] == '/') {
                iPos += 2;
                break;
              }
            
              iPos++;
            }
            break;

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
        switch (str[iPos]) {
          case '=': // &=
            iPos++;
            AddLdsToken(LTK_SET, iPrintPos, LOP_B_AND);
            break;

          case '&': // &&
            iPos++;
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_AND);
            break;

          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_B_AND);
        }
        break;

      case '^':
        switch (str[iPos]) {
          case '=': // ^=
            iPos++;
            AddLdsToken(LTK_SET, iPrintPos, LOP_B_XOR);
            break;

          case '^': // ^^
            iPos++;
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_XOR);
            break;

          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_B_XOR);
        }
        break;

      case '|':
        switch (str[iPos]) {
          case '=': // |=
            iPos++;
            AddLdsToken(LTK_SET, iPrintPos, LOP_B_OR);
            break;

          case '|': // ||
            iPos++;
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_OR);
            break;

          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_B_OR);
        }
        break;

      // conditional operators
      case '>':
        switch (str[iPos]) {
          case '=': // >=
            iPos++;
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_GOE);
            break;

          case '>': // >>
            iPos++;
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_SH_R);
            break;

          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_GT);
        }
        break;

      case '<':
        switch (str[iPos]) {
          case '=': // <=
            iPos++;
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_LOE);
            break;

          case '<': // <<
            iPos++;
            AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_SH_L);
            break;

          default: AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_LT);
        }
        break;

      case '=':
        // ==
        if (str[iPos] == '=') {
          iPos++;
          AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_EQ);
        } else {
          AddLdsToken(LTK_SET, iPrintPos, LOP_SET);
        }
        break;

      case '!':
        // !=
        if (str[iPos] == '=') {
          iPos++;
          AddLdsToken(LTK_OPERATOR, iPrintPos, LOP_NEQ);
        } else {
          AddLdsToken(LTK_UNARYOP, iPrintPos, UOP_INVERT);
        }
        break;

      // string
      case '"':
        while (iPos < ctLen) {
          if (str[iPos] == '"') {
            break;
          }
          iPos++;
        }

        if (iPos < ctLen) {
          string strString = str.substr(iStart+1, iPos - iStart-1);
          AddLdsToken(LTK_VAL, iPrintPos, strString);

          iPos++;

        } else {
          LdsThrow(LEP_STRING, "Unclosed string starting at %s", LdsPrintPos(iPrintPos).c_str());
        }
        break;
      
      // thread directive
      case '#':
        iChar = str[iPos];
        
        // start directive names with an underscore or letters
        if (iChar == '_'
        || (iChar >= 'a' && iChar <= 'z')
        || (iChar >= 'A' && iChar <= 'Z')) {
          // parse name symbols
          while (iPos < ctLen) {
            iChar = str[iPos];
            
            // allow underscore, letters and numbers
            if (iChar == '_'
            || (iChar >= '0' && iChar <= '9')
            || (iChar >= 'a' && iChar <= 'z')
            || (iChar >= 'A' && iChar <= 'Z')) {
              iPos++;
              
            // invalid symbol
            } else {
              break;
            }
          }
          
          // copy the name (excluding '#')
          string strDir = str.substr(iStart+1, iPos - iStart - 1);
          
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
        break;

      // constants
      default:
        if (iChar >= '0' && iChar <= '9') {
          char ubType = 0; // 1 - float, 2 - hex
          bool bHexCheck = (iChar == '0');
          
          // parse through numbers
          while (iPos < ctLen) {
            iChar = str[iPos];

            // change type
            if (iChar == 'x' || iChar == 'X') {
              // can't be a hexadecimal number anymore
              if (!bHexCheck) {
                break;
              }
              
              // register as a hexadecimal number
              ubType = 2;
              iPos++;

            // encountered a dot
            } else if (iChar == '.') {
              // already changed type
              if (ubType != 0) {
                break;
              }
              
              // register as a float
              ubType = 1;
              iPos++;
              
            // copy other numbers
            } else if (iChar >= '0' && iChar <= '9') {
              iPos++;

            // hexadecimal numbers
            } else if (ubType == 2) { 
              if ((iChar >= 'a' && iChar <= 'f') || (iChar >= 'A' && iChar <= 'F')) {
                iPos++;
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
          string strString = str.substr(iStart, iPos - iStart);
          
          switch (ubType) {
            case 0: { // index
              int iValue = atoi(strString.c_str());
              AddLdsToken(LTK_VAL, iPrintPos, iValue);
            } break;

            case 2: { // hexadecimal index
              int iHexValue = 0;
              sscanf(strString.c_str(), "%x", &iHexValue);
              AddLdsToken(LTK_VAL, iPrintPos, iHexValue);
            } break;

            default: { // float
              float fValue = atof(strString.c_str());
              AddLdsToken(LTK_VAL, iPrintPos, fValue);
            }
          }
        
        // start identifier names with an underscore or letters
        } else if (iChar == '_'
               || (iChar >= 'a' && iChar <= 'z')
               || (iChar >= 'A' && iChar <= 'Z')) {
          // parse name symbols
          while (iPos < ctLen) {
            iChar = str[iPos];
            
            // allow underscore, letters and numbers
            if (iChar == '_'
            || (iChar >= '0' && iChar <= '9')
            || (iChar >= 'a' && iChar <= 'z')
            || (iChar >= 'A' && iChar <= 'Z')) {
              iPos++;
              
            // invalid symbol
            } else {
              break;
            }
          }
          
          // copy the name
          string strName = str.substr(iStart, iPos - iStart);
          
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
            AddLdsToken(LTK_VAL, iPrintPos, 3.14159265359f);

          } else if (strName == "e") {
            AddLdsToken(LTK_VAL, iPrintPos, 2.71828182845f);

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

          } else {
            AddLdsToken(LTK_ID, iPrintPos, strName);
          }
        
        // unknown characters
        } else {
          LdsThrow(LEP_CHAR, "Unexpected character '%c' at %s", iChar, LdsPrintPos(iPrintPos).c_str());
        }
    }
  }

  AddLdsToken(LTK_END, ctLen);
};

// Add expression token
void CLdsScriptEngine::AddLdsToken(const ELdsToken &eType, const int &iPos) {
  _aetTokens.Add(CLdsToken(eType, iPos, -1));
};

void CLdsScriptEngine::AddLdsToken(const ELdsToken &eType, const int &iPos, const int &iValue) {
  _aetTokens.Add(CLdsToken(eType, iPos, iValue, -1));
};

void CLdsScriptEngine::AddLdsToken(const ELdsToken &eType, const int &iPos, const float &fValue) {
  _aetTokens.Add(CLdsToken(eType, iPos, fValue, -1));
};

void CLdsScriptEngine::AddLdsToken(const ELdsToken &eType, const int &iPos, const string &strValue) {
  _aetTokens.Add(CLdsToken(eType, iPos, strValue, -1));
};
