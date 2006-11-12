#ifndef _COLORER_VTList
#define _COLORER_VTList

#include<colorer/parsers/dynamic/DynamicHRCModel.h>

/**
 * Dynamic parser's list of virtual entries.
 * @ingroup colorer_parsers
 */
class VTList
{
  VirtualEntryVector *vlist;
  VTList *prev, *next, *last, *shadowlast;
  int nodesnum;
  
  //temporary??
  friend class HRCCompiler;

public:
  
  VTList() {
    vlist = null;
    prev = next = null;
    last = this;
    shadowlast = null;
    nodesnum = 0;
  }

  ~VTList() {
    assert(next != this);
    // deletes only from root
    if (!prev && next) next->deltree();
  }
  
  void deltree() {
    if (next)
      next->deltree();
    delete this;
  }

  bool push(SchemeNode *node)
  {
    VTList *newitem;
    if(!node || node->virtualEntryVector.size() == 0) return false;
    newitem = new VTList();
    if(last->next){
      last->next->prev = newitem;
      newitem->next = last->next;
    };
    newitem->prev = last;
    last->next = newitem;
    last = last->next;
    last->vlist = &node->virtualEntryVector;
    nodesnum++;
    return true;
  }

  bool pop()
  {
    VTList *ditem;

    assert(last != this);
    
    ditem = last;
    if (ditem->next){
      ditem->next->prev = ditem->prev;
    };
    ditem->prev->next = ditem->next;
    last = ditem->prev;
    delete ditem;
    nodesnum--;
    return true;
  }

  SchemeImpl *pushvirt(SchemeImpl *scheme)
  {
    SchemeImpl *ret = scheme;
    VTList *curvl = 0;

    for(VTList *vl = last; vl && vl->prev; vl = vl->prev){
      for(int idx = 0; idx < vl->vlist->size(); idx++){
        VirtualEntry *ve = vl->vlist->elementAt(idx);
        if (ret == ve->virtScheme && ve->substScheme){
          ret = ve->substScheme;
          curvl = vl;
        };
      };
    };
    if (curvl){
      curvl->shadowlast = last;
      last = curvl->prev;
      return ret;
    };
    return 0;
  }

  void popvirt()
  {
    VTList *that = last->next;
    assert(last->next && that->shadowlast);
    last = that->shadowlast;
    that->shadowlast = null;
  }

  void clear()
  {
    nodesnum = 0;
    if (!prev && next){
      next->deltree();
      next = 0;
    };
    last = this;
  }

  VirtualEntryVector **store()
  {
    VirtualEntryVector **store;
    int i = 0;
    
    if (!nodesnum || last == this) return null;
    store = new VirtualEntryVector*[nodesnum + 1];
    for(VTList *list = this->next; list; list = list->next){
      store[i++] = list->vlist;
      if (list == this->last) break;
    };
    store[i] = 0;
    return store;
  }

  bool restore(VirtualEntryVector **store)
  {
    VTList *prevpos, *pos = this;
    
    if (store == null) return false;
    assert(!next || next == this);
    assert(!prev || prev == this);
    //nodesnum = store[0].shadowlast;
    prevpos = last = 0;
    for(int i = 0; store[i] != null; i++){
      pos->next = new VTList;
      prevpos = pos;
      pos = pos->next;
      pos->prev = prevpos;
      pos->vlist = store[i];
      nodesnum++;
    };
    last = pos;
    return true;
  }

};

#endif
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
 * Portions created by the Initial Developer are Copyright (C) 1999-2006
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