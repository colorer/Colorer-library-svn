// creates object tree structure on html/xml files

#include<xml/xml.h>
#include<unicode/UnicodeTools.h>
#include<xml/xmlstrings.h>

CXmlEl::CXmlEl()
{
  eparent= 0;
  enext  = 0;
  eprev  = 0;
  echild = 0;
  chnum  = 0;
  type   = EL_ROOT;
  name   = 0;
  content= 0;
};
CXmlEl::~CXmlEl()
{
  if (type == EL_ROOT) enext->destroyLevel();
  if (echild) echild->destroyLevel();
  if (name) delete name;
  if (content) delete content;
  for (int i = 0; i < params.size(); i++){
    delete params.get(params.key(i));
  };
};
CXmlEl *CXmlEl::createNew(ElType type, CXmlEl *parent, CXmlEl *after)
{
  CXmlEl *el = new CXmlEl;
  el->type = type;
  if (parent){
    el->enext = parent->echild;
    el->eparent = parent;
    if (parent->echild) parent->echild->eprev = el;
    parent->echild = el;
    parent->chnum++;
    parent->type = EL_BLOCKED;
  }else{
    if (after) after->insert(el);
  };
  return el;
};
bool CXmlEl::init()
{
  return true;
};

bool CXmlEl::parse(const byte *src_byte, int sz, const char *codepage){
CXmlEl *child, *parent, *next = 0;
int i, j, lins, line;
int ls, le, rs, re, empty;

  // start object always root node
  type = EL_ROOT;
  next = this;

  DString src(src_byte, sz, Encodings::getEncodingIndex(codepage));

  sz = src.length();
  lins = line = 0;
  for (i = 0; i < sz; i++){
    if (i >= sz) continue;
    line = i;

    if (src[i] == '<' || i >= sz-1){
      while(line > lins){
#ifndef SAVE_WHITESPACES
        // drops linear whitespace text regions
        j = lins;
        while(Character::isWhitespace(src[j]) && j < i){
          j++;
        };
        if(j == i) break;
#endif
        child = createNew(EL_PLAIN, 0, next);
        child->init();
        child->content = new EntityString(&src, lins, line-lins);
        next = child;
        break;
      };
      lins = i;
      if (i == sz-1) continue;

      if (src.startsWith(DString("<!--"), i)){
        // comments parsing
        i += 4;
        int ie = src.indexOf(DString("-->"), i);
        if (ie == -1) ie = src.length()-3;
        i = ie + 3;
#ifdef SAVE_COMMENTS
        child = createNew(EL_COMMENT, 0, next);
        child->init();
        child->content = new SString(&src, lins, i-lins);
        next = child;
#endif
        lins = i--;
      }else if (src.startsWith(DString("<![CDATA["), i)){
        // CDATA section
        i += 9;
        int ie = src.indexOf(DString("]]>"), i);
        if (ie == -1) ie = src.length()-3;
        i = ie + 3;
        child = createNew(EL_PLAIN, 0, next);
        child->init();
        child->content = new SString(&src, lins+9, i-lins-9-3);
        next = child;
        lins = i--;
      }else if (src[i+1] != '/'){
        // start or single tag
        child = createNew(EL_SINGLE, 0, next);
        child->init();
        next  = child;
        j = i+1;
        while ( i < sz &&src[i] != '>' && !Character::isWhitespace(src[i])) i++;
        if (i-j > MAXTAG) i = j + MAXTAG - 1;
        child->name = new SString(&src, j, i-j);
        // parameters
        while(i < sz && src[i] != '>'){
          ls = i;
          while (ls < sz && Character::isWhitespace(src[ls])) ls++;
          le = ls;
          while (le < sz && !Character::isWhitespace(src[le]) && src[le]!='>' && src[le]!='=') le++;
          rs = le;
          while (rs < sz && Character::isWhitespace(src[rs])) rs++;
          empty = 1;

          if (rs < sz && src[rs] == '='){
            empty = 0;
            rs++;
            while (rs < sz && Character::isWhitespace(src[rs])) rs++;
            re = rs;
            if (src[re] == '"'){
              while(re < sz && src[++re] != '"');
              rs++;
              i = re+1;
            }else if (src[re] == '\''){
              while(re < sz && src[++re] != '\'');
              rs++;
              i = re+1;
            }else{
              i = re;
              continue;
            };
          }else{
            i = rs;
            continue;
          };
          if (ls == le) continue;
          child->params.put(&DString(&src, ls, le-ls), new EntityString(&src, rs, re-rs));
        };
        lins = i+1;
      }else{  // end tag
        j = i+2;
        i+=2;
        while (i < sz && src[i] != '>' && !Character::isWhitespace(src[i])) i++;
        int cn = 0;
        for(parent = next; parent->eprev; parent = parent->eprev, cn++){
          if(!parent->name) continue;
          int len = parent->name->length();
          if (len != i-j) continue;
          if (parent->type != EL_SINGLE ||
              DString(&src, j, len) != *parent->name) continue;
          break;
        };
        if(parent && parent->eprev){
          parent->echild = parent->enext;
          parent->chnum = cn;
          parent->type = EL_BLOCKED;
          child = parent->echild;
          if (child) child->eprev = 0;
          while(child){
            child->eparent = parent;
            child = child->enext;
          };
          parent->enext = 0;
          next = parent;
        };
        while(i < sz && src[i] != '>') i++;
        lins = i+1;
      };
    };
  };
////
  return true;
};

void CXmlEl::insert(CXmlEl *el)
{
  el->eprev = this;
  el->enext = this->enext;
  el->eparent = this->eparent;
  if (this->enext) this->enext->eprev = el;
  this->enext = el;
};
// recursive drop
void CXmlEl::destroyLevel()
{
  if (enext) enext->destroyLevel();
  delete this;
};

CXmlEl *CXmlEl::parent()
{
  return eparent;
};
CXmlEl *CXmlEl::next()
{
  return enext;
};
CXmlEl *CXmlEl::prev()
{
  return eprev;
};
CXmlEl *CXmlEl::child()
{
  return echild;
};
ElType CXmlEl::getType()
{
  return type;
};
const String *CXmlEl::getName()
{
  if (!name || !name->length()) return 0;
  return name;
};
const String *CXmlEl::getContent()
{
  return content;
};

int CXmlEl::getParamCount()
{
  return params.size();
};
const String* CXmlEl::getParamName(int no)
{
  return params.key(no);
};
const String* CXmlEl::getParamValue(int no)
{
  return params.get(params.key(no));;
};
const String* CXmlEl::getParamValue(const String &par)
{
  return params.get(&par);
};

bool CXmlEl::getParamValue(const String &par, int *result)
{
double res = 0;
const String *val = params.get(&par);
  if (val == null) return false;
  bool b = UnicodeTools::getNumber(val, &res);
  if (b) *result = (int)res;
  return b;
};

bool CXmlEl::getParamValue(const String &par, double *result)
{
double res;
const String *val = params.get(&par);
  if (val == null) return false;
  bool b = UnicodeTools::getNumber(val, &res);
  if (b) *result = (double)res;
  return b;
};

CXmlEl *CXmlEl::search(const String &tagname)
{
CXmlEl *next = this->enext;
  while(next){
    if (tagname == *next->name) return next;
    next = next->enext;
  };
  return next;
};

CXmlEl *CXmlEl::enumChildred(int no)
{
CXmlEl *el = this->echild;
  while(no && el){
    el = el->enext;
    no--;
  };
  return el;
};

CXmlEl *CXmlEl::fPrev()
{
CXmlEl *el = this;
  if (!el->eprev) return el->eparent;
  if (el->eprev->echild)
    return el->eprev->echild->fLast();
  return el->eprev;
};
CXmlEl *CXmlEl::fNext()
{
CXmlEl *el = this;
  if (el->echild) return el->echild;
  while(!el->enext){
    el = el->eparent;
    if (!el) return 0;
  };
  return el->enext;
};
CXmlEl *CXmlEl::fFirst()
{
CXmlEl *prev = this;
  while(prev){
    if (!prev->eprev) return prev;
    prev = prev->eprev;
  };
  return prev;
};

CXmlEl *CXmlEl::fLast()
{
CXmlEl *nxt = this;
  while(nxt->enext || nxt->echild){
    if (nxt->enext){
      nxt = nxt->enext;
      continue;
    };
    if (nxt->echild){
      nxt = nxt->echild;
      continue;
    };
  };
  return nxt;
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
 * Cail Lomecb <ruiv@uic.nnov.ru>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2003
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
