package net.sf.colorer;

import java.util.*;
import net.sf.colorer.handlers.*;

public class ParserFactory{
  static{
    System.loadLibrary("net_sf_colorer");
  };
  long iptr;
  long internal;
  static int count = 0;

  public ParserFactory(String catalogPath){
    iptr = init(catalogPath);
    internal = iptr;
    System.out.println("ParserFactory count: " + (++count) + " iptr="+iptr);
  };
  public ParserFactory(){
    iptr = init(null);
    internal = iptr;
    System.out.println("ParserFactory(init) count: " + (++count) + " iptr="+iptr);
  };
  protected void finalize() throws Throwable{
    System.out.println("ParserFactory(fin) try internal="+internal);
    if (internal != 0) System.out.println("ParserFactory(fin) count: " + (--count));
    finalize(internal);
  };

  public String getVersion(){
    return getVersion(iptr);
  };

  public Enumeration enumerateHRDClasses(){
    return new Enumeration(){
      int idx = 0;
      public Object nextElement(){
        String cls = enumerateHRDClasses(iptr, idx);
        if (cls == null) return null;
        idx++;
        return cls;
      };
      public boolean hasMoreElements(){
        String cls = enumerateHRDClasses(iptr, idx);
        return (cls != null);
      };
    };
  };
  public Enumeration enumerateHRDInstances(String classID){
    return new HRDEnumeration(classID);
  }
  class HRDEnumeration implements Enumeration{
    int idx = 0;
    String classID = null;
    HRDEnumeration(String classID){
      this.classID = classID;
    };
    public Object nextElement(){
      String cls = enumerateHRDInstances(iptr, classID, idx);
      if (cls == null) return null;
      idx++;
      return cls;
    };
    public boolean hasMoreElements(){
      String cls = enumerateHRDInstances(iptr, classID, idx);
      return (cls != null);
    };
  };

  public String getHRDescription(String classID, String nameID){
    return getHRDescription(iptr, classID, nameID);
  };

  public HRCParser getHRCParser(){
    return getHRCParser(iptr);
  };

  public TextParser createTextParser(){
    return createTextParser(iptr);
  }

  public RegionMapper createStyledMapper(String classID, String nameID)
      throws ParserFactoryException{
    return createStyledMapper(iptr, classID, nameID);
  };
  public RegionMapper createTextMapper(String classID, String nameID)
      throws ParserFactoryException{
    return createTextMapper(iptr, nameID);
  };

  native long init(String catalogPath);
  native void finalize(long iptr);
  native String getVersion(long iptr);

  native String enumerateHRDClasses(long iptr, int idx);
  native String enumerateHRDInstances(long iptr, String hrdClass, int idx);
  native String getHRDescription(long iptr, String classID, String nameID);
  native HRCParser getHRCParser(long iptr);
  native TextParser createTextParser(long iptr);
  native RegionMapper createStyledMapper(long iptr, String classID, String nameID);
  native RegionMapper createTextMapper(long iptr, String nameID);
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