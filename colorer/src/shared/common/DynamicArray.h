#ifndef _COLORER_DYNAMICARRAY_H
#define _COLORER_DYNAMICARRAY_H

#include<common/Exception.h>

#define DEFAULT_DYNAMICARRAY_CAPACITY 20

/** Ordered sequence of objects.
    Each object has it's ordinal position.
    @ingroup common
*/
template <class T>
class DynamicArray{
public:
  
  /** Default Constructor
  */
  DynamicArray()
  {
      csize = 0;
      incrementSize = 0;
      asize = DEFAULT_DYNAMICARRAY_CAPACITY;
      array = new T[asize];
  }

  /** Constructor with explicit vector size specification
      @param initsize Initial vector size.
      @param incrementSize Positive number, which should be added to vector size
             each time it reaches it maximum capacity.
             If zero, internal array, each time filled, is double sized.
  */
  DynamicArray(int initsize, int incrementSize = 0)
  {
      csize = 0;
      asize = initsize;
      this->incrementSize = incrementSize;
      array = new T[asize];
  };

  /** Default Destructor
  */
  ~DynamicArray()
  {
      delete[] array;
  }

  /** Clears vector.
  */
  void clear()
  {
      csize = 0;
  }
  
  /** Returns number of elements, stored in vector
  */
  int size() const
  {
      return csize;
  };
  
  /** Changes vector size. if @c newSize is more, than current size,
      vector is expanded with requested number of elements, and set these
      elements to null. If @c newSize is less, than current size,
      number of top elements is deleted from it.
  */
  void setSize(int newSize)
  {
      if (newSize < 0) throw OutOfBoundException();
      if (newSize <= csize){
        csize = newSize;
        return;
      };
      if (newSize > asize) ensureCapacity(newSize);
      if (newSize <= asize){
        for(int idx = csize; idx < newSize; idx++)
          array[idx] = null; //!!!
        csize = newSize;
      };
  }
  
  /** Ensures, that vector can store specified number of elements
      without resizing.
  */
  void ensureCapacity(int minCapacity)
  {
      if (asize >= minCapacity) return;
      T* newarray = new T[minCapacity];
      asize = minCapacity;
      for(int idx = 0; idx < csize; idx++)
        newarray[idx] = array[idx];
      delete[] array;
      array = newarray;
  };

  /** Adds element into tail of sequence.
  */
  T *addElement()
  {
    return insertElementAt(csize);
  }
  
  /** Inserts element at specified position and expand vector
      by one element.
  */
  T *insertElementAt(int index)
  {
      if (index < 0 || index > csize) throw OutOfBoundException(SString(index));

      if (index == csize && asize > csize){
        csize++;
        return &array[index];
      };
      if (asize > csize){
        for (int i = csize; i > index; i--)
          array[i] = array[i-1];
        csize++;
        return &array[index];
      };
      
      T *retval = null;
      if (incrementSize == 0) asize = asize*2;
      else asize += incrementSize;
      T *newarray = new T[asize];
      int nidx = 0;
      for (int oidx = 0; oidx < csize; oidx++, nidx++){
        if (oidx == index) retval = &newarray[nidx++];
        newarray[nidx] = array[oidx];
      };
      if (index == csize) retval = &newarray[csize];
      csize++;
      delete[] array;
      array = newarray;
      return retval;
  };
  
  /** Removes element at specified @c index and shift all
      elements.
  */
  void removeElementAt(int index)
  {
      if (index < 0 || index >= csize) throw OutOfBoundException(SString(index));
      for(int idx = index; idx < csize-1; idx++)
        array[idx] = array[idx+1];
      csize--;
  };
  
  /** Returns element at specified position
      @throw OutOfBoundException If @c index is too big or less, than zero.
  */
  T *elementAt(int index) const
  {
      if (index < 0 || index >= csize) throw OutOfBoundException(SString(index));
      return &array[index];
  };
  
  /** Returns last element of vector.
      @throw OutOfBoundException If vector has no elements.
  */
  T *lastElement() const
  {
      if (csize == 0) throw OutOfBoundException(DString("no lastElement in empty vector"));
      return &array[csize-1];
  };
private:
  int csize, asize;
  int incrementSize;
  T*  array;
  DynamicArray &operator=(DynamicArray&);
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
