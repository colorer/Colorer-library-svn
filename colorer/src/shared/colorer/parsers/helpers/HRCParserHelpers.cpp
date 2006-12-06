
#include<colorer/parsers/helpers/HRCParserHelpers.h>
#include<stdlib.h>

KeywordList::KeywordList()
{
  num = 0;
  matchCase = false;
  minKeywordLength = 0;
  kwList = null;
  worddiv = null;
  firstChar = new CharacterClass();
};
KeywordList::~KeywordList(){
  for(int idx = 0; idx < num; idx++) {
    delete kwList[idx].keyword;
  }
  delete worddiv;
  delete[] kwList;
  delete   firstChar;
};

int kwCompare(const void*e1, const void*e2){
  return ((KeywordInfo*)e1)->keyword->compareTo(*((KeywordInfo*)e2)->keyword);
};
int kwCompareI(const void*e1, const void*e2){
  return ((KeywordInfo*)e1)->keyword->compareToIgnoreCase(*((KeywordInfo*)e2)->keyword);
};
void KeywordList::sortList(){
  if (num < 2) return;

  if (matchCase) qsort((void*)kwList, num, sizeof(KeywordInfo), &kwCompare);
  else qsort((void*)kwList, num, sizeof(KeywordInfo), &kwCompareI);
};

/* Searches previous elements num with same partial name
fe:
3: getParameterName  0
2: getParameter      0
1: getParam          0
0: getPar            -1

*/
void KeywordList::substrIndex(){
  for(int i = num-1; i > 0; i--)
    for(int ii = i-1; ii != 0; ii--){
      if ((*kwList[ii].keyword)[0] != (*kwList[i].keyword)[0]) break;
      if (kwList[ii].keyword->length() < kwList[i].keyword->length() &&
          DString(kwList[i].keyword, 0, kwList[ii].keyword->length()) == *kwList[ii].keyword){
        kwList[i].ssShorter = ii;
        break;
      };
    };
};

char* schemeNodeTypeNames[] =  { "EMPTY", "RE", "SCHEME", "KEYWORDS", "INHERIT" };


SchemeNode::SchemeNode() : virtualEntryVector(5), regionPairVector(4) {
  type = SNT_EMPTY;
  schemeName = null;
  scheme = null;
  kwList = null;
  start = end = null;
  start_re = end_re = null;
  lowPriority = 0;

  region = null;
  memset(regions, 0, sizeof(regions));
  memset(regionsn, 0, sizeof(regionsn));
  memset(regione, 0, sizeof(regione));
  memset(regionen, 0, sizeof(regionen));
}

SchemeNode::~SchemeNode(){
  if (type == SNT_RE || type == SNT_SCHEME){
    delete start;
    delete end;
    delete start_re;
    delete end_re;
  }
  if (type == SNT_KEYWORDS){
    delete kwList;
  }
  if (type == SNT_INHERIT){
    for(int idx = 0; idx < virtualEntryVector.size(); idx++)
      delete virtualEntryVector.elementAt(idx);
  }
  delete schemeName;
}


/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Colorer Library.
 *
 * The Initial Developer of the Original Code is
 * Igor Russkih <irusskih at gmail.com>
 * Portions created by the Initial Developer are Copyright (C) 1999-2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
