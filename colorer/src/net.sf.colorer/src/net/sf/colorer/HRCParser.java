package net.sf.colorer;

import java.util.*;

/**
    Abstract template of HRCParser class implementation.
    Defines basic operations of loading and accessing HRC information.
*/
public class HRCParser{
  long iptr;
  static int count = 0;

  HRCParser(long _iptr){
    iptr = _iptr;
  }
  protected void finalize() throws Throwable {
    finalize(iptr);
  }
  native void finalize(long iptr);

  /** Returns reference to region with specified qualified
      name. If no such region, returns null.
  */
  public Region getRegion(String qname){
    return getRegion(iptr, qname);
  }
  native Region getRegion(long iptr, String qname);


  /** Enumerates all available language types.
      Each element in enumeration contains reference to a
      FileType object instance.
  */
  public Enumeration enumerateFileTypes(){
    return new Enumeration(){
      int idx = 0;
      public boolean hasMoreElements() {
        FileType cls = enumerateFileTypes(iptr, idx);
        return (cls != null);
      }
      public Object nextElement() {
        FileType cls = enumerateFileTypes(iptr, idx);
        if (cls == null) return null;
        idx++;
        return cls;
      }
    };
  }
  native FileType enumerateFileTypes(long iptr, int idx);

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
 * Cail Lomecb <cail@nm.ru>.
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