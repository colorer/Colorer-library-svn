package net.sf.colorer.impl;

import net.sf.colorer.*;
import net.sf.colorer.editor.BaseEditor;
import net.sf.colorer.editor.PairMatch;
import net.sf.colorer.handlers.*;

public class BaseEditorNative implements BaseEditor {
  // internal native object
  private long iptr;
  
  private int wStart, wSize;
  private int lineCount;
  
  private Region defPairStart = null;
  private Region defPairEnd = null;

  native Region getRegion(final long iptr, final String qname);

  public BaseEditorNative(ParserFactory pf, LineSource lineSource){
    iptr = init(pf, lineSource);
    HRCParser hrcParser = pf.getHRCParser();
    defPairStart = hrcParser.getRegion("def:PairStart");
    defPairEnd = hrcParser.getRegion("def:PairEnd");
    setBackParse(2000);// TODO!!!
  };
  protected void finalize() throws Throwable{
    finalize(iptr);
  }

  public void setRegionCompact(boolean compact){
    setRegionCompact(iptr, compact);
  }
  public void setFileType(String typename){
    setFileType(iptr, typename);
    modifyEvent(iptr, 0);
  }
  public String chooseFileType(String fname){
    modifyEvent(iptr, 0);
    return chooseFileType(iptr, fname);
  }
  public String getFileType(){
    return getFileType(iptr);
  }
  public void setRegionMapper(String cls, String name){
    setRegionMapper(iptr, cls, name);
    modifyEvent(iptr, 0);
  }
  public void addRegionHandler(RegionHandler rh, Region filter){
    addRegionHandler(iptr, rh, filter);
  }
  public void removeRegionHandler(RegionHandler rh){
    removeRegionHandler(iptr, rh);
  }

  public RegionDefine getBackground(){
    return getBackground(iptr);
  }
  public RegionDefine getVertCross(){
    return getVertCross(iptr);
  };
  public RegionDefine getHorzCross(){
    return getHorzCross(iptr);
  }

  public void setBackParse(int backParse) {
    setBackParse(iptr, backParse);
  }

  public PairMatch getPairMatch(int lineNo, int linePos){
    LineRegion[] lrArray = getLineRegions(iptr, lineNo);
    if (lrArray.length == 0) return null;
    
    LineRegion pair = null;
    for(int idx = 0; idx < lrArray.length; idx++){
      LineRegion l1 = lrArray[idx];
      if (l1.region == null) continue;
      if ((l1.region.hasParent(defPairStart) ||
           l1.region.hasParent(defPairEnd)) &&
           linePos >= l1.start && linePos <= l1.end)
        pair = l1;
    };
    if (pair != null){
      PairMatch pm = new PairMatch(pair, null, lineNo, -1, -1, false);
      if (pair.region.hasParent(defPairStart)){
        pm.pairBalance = 1;
        pm.topPosition = true;
      };
      return pm;
    };
    return null;
  }
  
  int getLastVisibleLine(){
    int r1 = (wStart+wSize);
    int r2 = lineCount;
    return ((r1 > r2)?r2:r1)-1;
  }
  void searchPair(PairMatch pm, int start_line, int end_line){
    LineRegion pair = pm.start;
    int lno = pm.sline;
    LineRegion[] slr = getLineRegions(lno);
    int li = 0;
    while(li < slr.length && slr[li] != pair) li++;
    if (li == slr.length) return;

    while(true){
      if (pm.pairBalance > 0){
        li++;
        while(li == slr.length){
          lno++;
          if (lno > end_line) break;
          slr = getLineRegions(lno);
          li = 0;
        };
        if (lno > end_line) break;
        pair = slr[li];
      }else{
        if(li == 0){
          lno--;
          if (lno < start_line) break;
          slr = getLineRegions(lno);
          li = slr.length;
        };
        if (lno < start_line) break;
        li--;
        pair = slr[li];
      };
      if (pair.region == null) continue;
      if (pair.region.hasParent(defPairStart)) pm.pairBalance++;
      if (pair.region.hasParent(defPairEnd)) pm.pairBalance--;
      if (pm.pairBalance == 0) break;
    };
    if (pm.pairBalance == 0){
      pm.eline = lno;
      pm.end = pair;
    };
  };
  public void searchLocalPair(PairMatch pm){
    int end_line = getLastVisibleLine();
    searchPair(pm, wStart, end_line);
  }
  
  public void searchGlobalPair(PairMatch pm){
    searchPair(pm, 0, lineCount-1);
  }
  public LineRegion[] getLineRegions(int lno){
    return getLineRegions(iptr, lno);
  }
  public void validate(int lno) {
    validate(iptr, lno);
   }
  public void idleJob(int time) {
    idleJob(iptr, time);
  }
  public void modifyEvent(int topLine){
    modifyEvent(iptr, topLine);
  }
  public void modifyLineEvent(int line){
    modifyLineEvent(iptr, line);
  }
  public void visibleTextEvent(int wStart, int wSize){
    visibleTextEvent(iptr, wStart, wSize);
    this.wStart = wStart;
    this.wSize = wSize;
  }
  public void lineCountEvent(int newLineCount){
    lineCountEvent(iptr, newLineCount);
    lineCount = newLineCount;
  }

  private native long init(ParserFactory pf, LineSource lineSource);
  private native void finalize(long iptr);
  
  public native void setRegionCompact(long iptr, boolean compact);
  public native void setRegionMapper(long iptr, String cls, String name);

  public native void addRegionHandler(long iptr, RegionHandler rh, Region filter);
  public native void removeRegionHandler(long iptr, RegionHandler rh);

  public native void setFileType(long iptr, String typename);
  public native String chooseFileType(long iptr, String fname);
  public native String getFileType(long iptr);

  public native void setBackParse(long iptr, int backParse);
  
  public native RegionDefine getBackground(long iptr);
  public native RegionDefine getVertCross(long iptr);
  public native RegionDefine getHorzCross(long iptr);
  
  public native LineRegion[] getLineRegions(long iptr, int lno);
  public native void validate(long iptr, int lno);
  public native void idleJob(long iptr, int time);

  public native void modifyEvent(long iptr, int topLine);
  public native void modifyLineEvent(long iptr, int line);
  public native void visibleTextEvent(long iptr, int wStart, int wSize);
  public native void lineCountEvent(long iptr, int newLineCount);

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