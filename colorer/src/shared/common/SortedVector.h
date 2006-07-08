#ifndef _COLORER_SORTEDVECTOR_H_
#define _COLORER_SORTEDVECTOR_H_

#include<common/Vector.h>

/**
 * Automatically sorted sequence of objects.
 * Each object has it's ordinal position.
 * Request to search for the object uses binary search algorithm
 * @ingroup common
 */
template <class T>
class SortedVector : public Vector<T>{
public:

  SortedVector() : Vector<T>() { };

  SortedVector(int initsize, int incrementSize = 0) : Vector<T>(initsize, incrementSize) {};
  
  virtual ~SortedVector(){};

  virtual void addElement(const T el)
  {
    int idx = locateElement(el);
    Vector<T>::insertElementAt(el, idx);
  }
  
  virtual void insertElementAt(const T el, int index)
  {
    throw InvalidOperationException();
  }

  virtual void setElementAt(const T el, int index)
  {
    throw InvalidOperationException();
  }

  virtual bool removeElement(const T el)
  {
    int idx = locateElement(el);
    if (Vector<T>::elementAt(idx) != el){
      return false;
    }
    Vector<T>::removeElementAt(idx);
    return true;
  }

  virtual int indexOf(T el, int index) const
  {
    throw InvalidOperationException();
  }

  virtual int indexOf(T el) const
  {
    int idx = locateElement(el);
    if (idx < this->csize && Vector<T>::elementAt(idx) == el) {
      return idx;
    }else{
      return -1;
    }
  }

protected:
  /**
   * Searches for the element, if no element available,
   * returns the nearest position in sorted sequence.
   */
  int locateElement(T el) const {
    int s = 0;
    int e = this->size();
    int idx = 0;

    while (true) {
      idx = s + (e-s)/2;
      if (this->array[idx] == el){
        return idx;
      }else if (this->array[idx] > el){
        if (e-s <= 1) return s;
        e = idx;
      }else{  
        if (e-s <= 1) return e;
        s = idx;
      }
    }
    return idx;
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
 * Cail Lomecb <cail@nm.ru>.
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
