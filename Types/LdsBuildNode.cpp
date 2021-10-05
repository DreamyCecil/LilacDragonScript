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
#include "LdsBuildNode.h"

// Copy from another node
CBuildNode &CBuildNode::operator=(const CBuildNode &bnOther) {
  if (this == &bnOther) {
    return *this;
  }
      
  // delete old pointers
  RemoveReferences(-1);

  // copy parameters
  lt_eType = bnOther.lt_eType;
  lt_iPos = bnOther.lt_iPos;
  lt_iArg = bnOther.lt_iArg;
  lt_valValue = bnOther.lt_valValue;

  // copy referenced nodes
  CopyNodes(bnOther.bn_abnNodes);

  return *this;
};

// Add new node reference
void CBuildNode::AddReference(CBuildNode *pbn) {
  if (pbn != NULL) {
    bn_abnNodes.Add(new CBuildNode(*pbn));
  }
};

// Remove node references
void CBuildNode::RemoveReferences(int iPos) {
  // remove one node
  if (iPos != -1) {
    delete bn_abnNodes[iPos];
    bn_abnNodes.Delete(iPos);
    return;
  }

  // remove all nodes
  int ctNodes = bn_abnNodes.Count();

  while (ctNodes-- > 0) {
    delete bn_abnNodes[0];
    bn_abnNodes.Delete(0);
  }
};

// Copy nodes from some dynamic list
int CBuildNode::CopyNodes(const CDynamicNodeList &abnNodes) {
  int ctNodes = abnNodes.Count();

  for (int iNode = 0; iNode < ctNodes; iNode++) {
    AddReference(abnNodes[iNode]);
  }

  return ctNodes;
};
    
// Copy nodes from some list
int CBuildNode::CopyNodes(CNodeList &abnNodes) {
  int ctNodes = abnNodes.Count();

  for (int iNode = 0; iNode < ctNodes; iNode++) {
    AddReference(&abnNodes[iNode]);
  }

  return ctNodes;
};

// Print out the node
string CBuildNode::Print(string strTab) {
  // open this node block
  string str = strTab + _astrBuildNodes[lt_eType];
  str += " =={\n";

  string strNodeTab = strTab + ". ";

  // print node value
  string strValue = lt_valValue->Print();

  // special values
  switch (lt_eType) {
    case EBN_BINARY_OP:
    case EBN_ASSIGN_OP:
      strValue = _astrBinaryOps[lt_valValue->GetIndex()];
      break;

    case EBN_UNARY_OP:
      strValue = lt_valValue->GetString();
      break;
  }

  // surround strings with quotes
  if (lt_valValue->GetType() == EVT_STRING) {
    strValue = "\"" + strValue + "\"";
  }

  str += LdsPrintF("%sValue : %s\n", strNodeTab.c_str(), strValue.c_str());
      
  // print node argument
  string strArg = _astrBuildNodeArguments[lt_eType];

  if (strArg != "") {
    str += LdsPrintF("%s%s : %d\n", strNodeTab.c_str(), strArg.c_str(), lt_iArg);
  }

  // open nodes block
  if (bn_abnNodes.Count() > 0) {
    str += '\n';
    str += strNodeTab + "NODES -{\n";
  }

  // print each sub node
  for (int iNode = 0; iNode < bn_abnNodes.Count(); iNode++) {
    if (iNode != 0) {
      str += '\n';
    }

    str += bn_abnNodes[iNode]->Print(strNodeTab + ". ");
  }

  // close nodes block
  if (bn_abnNodes.Count() > 0) {
    str += strNodeTab + "}-\n";
  }
      
  // close this node block
  str += strTab + "}==\n";

  return str;
};