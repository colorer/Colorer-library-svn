#include<stdio.h>
#include<xml/xmledit.h>
#include<xml/xml.cpp>

// allows you to modify objects
CXmlEdit::CXmlEdit(){};
CXmlEdit::~CXmlEdit()
{
  if (eparent){
    eparent->chnum--;
    if (eparent->echild == this) eparent->echild = enext;
  };
  if (enext) enext->eprev = eprev;
  if (eprev) eprev->enext = enext;
};

void CXmlEdit::setName(const String *newname)
{
  if (name) delete name;
  name = new SString(newname);
};
bool CXmlEdit::addParam(const String *name, const String *val)
{
int i;
  if (parnum == MAXPARAMS) return false;
  i = parnum;
  parnum++;
  for (i = 0; i < parnum - 1; i++)
    if (*params[i][0] == *name){
      delete params[i][0];
      delete params[i][1];
      parnum--;
      break;
    };
  params[i][0] = new SString(name);
  params[i][1] = new SString(val);
  return true;
};
bool CXmlEdit::addParam(const String *name, int val)
{
char intval[20];
  sprintf(intval, "%d", val);
  return addParam(name, &DString(intval));
};
bool CXmlEdit::addParam(const String *name, double val)
{
char fltval[20];
  sprintf(fltval, "%.2f", val);
  return addParam(name, &DString(fltval));
};
bool CXmlEdit::removeParam(const String *name)
{
  for (int i = 0; i < parnum; i++)
    if (*params[i][0] == *name){
      delete params[i][0];
      delete params[i][1];
      params[i][0] = params[parnum-1][0];
      params[i][1] = params[parnum-1][1];
      parnum--;
      return true;
    };
  return false;
};
bool CXmlEdit::changeContent(const String *data)
{
  if (type != EL_PLAIN) return false;
  if (content) delete content;
  content = new SString(data);
  return true;
};

bool CXmlEdit::isLoop(CXmlEdit *el, CXmlEdit *parent)
{
  while(parent){
    if (el == parent) return true;
    parent = (PXmlEdit)parent->eparent;
  };
  return false;
};
bool CXmlEdit::move(CXmlEdit *parent, CXmlEdit *after)
{
  if (isLoop(this, parent)) return false;
  if (after && isLoop(this,(PXmlEdit)after->eparent)) return false;
  if (after){
    if (enext) enext->eprev = eprev;
    if (eprev) eprev->enext = enext;
    if (this->eparent->echild == this)
      this->eparent->echild = this->enext;
    this->eparent->chnum--;

    after->insert(this);
    this->eparent = after->eparent;
    if (this->eparent) this->eparent->chnum++;
    return true;
  }else
  if (parent){
    if (enext) enext->eprev = eprev;
    if (eprev) eprev->enext = enext;
    if (this->eparent->echild == this)
      this->eparent->echild = this->enext;
    this->eparent->chnum--;
    this->eparent = parent;
    enext = parent->echild;
    eprev = 0;
    this->eparent->echild = this;
    this->eparent->chnum++;
    if (enext) enext->eprev = this;
    return true;
  };
  return false;
};

int CXmlEdit::getLevelSize(int Lev)
{
int Pos = 0;
CXmlEdit *tmp = this;
  do{
    if (tmp->getType() != EL_PLAIN)
      Pos +=Lev*SP;
    if (tmp->getName())
      Pos += tmp->getName()->length()+1;
    for (int i = 0;i < tmp->getParamCount();i++){
      Pos += tmp->getParamName(i)->length()+2;
      Pos += tmp->getParamValue(i)->length()+2;
    };
    if (tmp->getName()) Pos +=3;
    if (tmp->getType() == EL_PLAIN && tmp->getContent())
      Pos += tmp->getContent()->length()+2;
    if (tmp->child())
      Pos += PXmlEdit(tmp->child())->getLevelSize(Lev+1);
    if (tmp->getType() == EL_BLOCKED && tmp->getName()){
      Pos += Lev*SP+5;
      Pos += tmp->getName()->length();
    };
    tmp = (PXmlEdit)tmp->next();
  }while(tmp);
  return Pos;
};

int CXmlEdit::saveLevel(char *Dest, int Lev)
{
int i,Pos = 0;
CXmlEdit *tmp = this;
  do{
    if (tmp->getType() != EL_PLAIN)
      for(i = 0; i < Lev*SP; i++) Pos += sprintf(Dest+Pos," ");
    if (tmp->getName())
      Pos += sprintf(Dest+Pos, "<%s", tmp->getName()->getChars());
    for (i = 0; i < tmp->getParamCount(); i++){
      Pos += sprintf(Dest+Pos, " %s=", tmp->getParamName(i)->getChars());
      Pos += sprintf(Dest+Pos, "\"%s\"", tmp->getParamValue(i)->getChars());
    };
    if (tmp->getName()) Pos += sprintf(Dest+Pos, ">");
    if (tmp->getType() == EL_PLAIN)
      Pos += sprintf(Dest+Pos, "%s", tmp->getContent()->getChars());
    if (tmp->child())
      Pos += PXmlEdit(tmp->child())->saveLevel(Dest+Pos, Lev+1);
    if (tmp->getType() == EL_BLOCKED){
      for(i = 0; i < Lev*SP; i++)
        Pos += sprintf(Dest+Pos, " ");
      Pos += sprintf(Dest+Pos, "</");
      if (tmp->getName()) Pos += sprintf(Dest+Pos,"%s",tmp->getName()->getChars());
      Pos += sprintf(Dest+Pos, ">");
    };
    tmp = (PXmlEdit)tmp->next();
  }while(tmp);
  return Pos;
};

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
