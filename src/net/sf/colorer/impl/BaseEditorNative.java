package net.sf.colorer.impl;

import java.util.Vector;

import net.sf.colorer.*;
import net.sf.colorer.editor.BaseEditor;
import net.sf.colorer.editor.EditorListener;
import net.sf.colorer.editor.PairMatch;
import net.sf.colorer.handlers.*;

public class BaseEditorNative implements BaseEditor {
    /** internal native object */
    private long iptr;
    private boolean disposed = false;
    private int wStart, wSize;
    private int lineCount;
    private Region defPairStart = null;
    private Region defPairEnd = null;
    private Vector editorListeners = new Vector(); 

    //native Region getRegion(final long iptr, final String qname);

    public BaseEditorNative(ParserFactory pf, LineSource lineSource) {
        iptr = init(pf, lineSource);
        HRCParser hrcParser = pf.getHRCParser();
        defPairStart = hrcParser.getRegion("def:PairStart");
        defPairEnd = hrcParser.getRegion("def:PairEnd");
        setBackParse(2000);// TODO!!!
        Logger.trace("BaseEditor", "init");
    };

    
    void checkActive() {
        if (disposed) {
            throw new RuntimeException("checkActive");
        }
    }
    
    public boolean isDisposed() {
        return disposed;
    }
    
    public void dispose() {
        checkActive();
        disposed = true;
        finalize(iptr);
        Logger.trace("BaseEditor", "dispose");
    }

    protected void finalize() throws Throwable {
        if (disposed) return;
        dispose();
    };

    public void setRegionCompact(boolean compact) {
        checkActive();
        setRegionCompact(iptr, compact);
    }

    public void setFileType(FileType typename) {
        checkActive();
        setFileType(iptr, typename);
        modifyEvent(iptr, 0);
    }

    public FileType chooseFileType(String fname) {
        checkActive();
        modifyEvent(iptr, 0);
        return chooseFileType(iptr, fname);
    }

    public FileType getFileType() {
        checkActive();
        return getFileType(iptr);
    }

    public void setRegionMapper(RegionMapper regionMapper) {
        checkActive();
        setRegionMapper(iptr, regionMapper);
        modifyEvent(iptr, 0);
    }

    public void setRegionMapper(String hrdClass, String hrdName) {
        checkActive();
        setRegionMapper(iptr, hrdClass, hrdName);
        modifyEvent(iptr, 0);
    }

    public void addRegionHandler(RegionHandler rh, Region filter) {
        checkActive();
        addRegionHandler(iptr, rh, filter);
    }
    public void removeRegionHandler(RegionHandler rh) {
        checkActive();
        removeRegionHandler(iptr, rh);
    }

    public void addEditorListener(EditorListener el) {
        checkActive();
        editorListeners.add(el);
    }
    public void removeEditorListener(EditorListener el) {
        checkActive();
        editorListeners.remove(el);
    }

    public RegionDefine getBackground() {
        checkActive();
        return getBackground(iptr);
    }

    public RegionDefine getVertCross() {
        checkActive();
        return getVertCross(iptr);
    };

    public RegionDefine getHorzCross() {
        checkActive();
        return getHorzCross(iptr);
    }

    public void setBackParse(int backParse) {
        checkActive();
        setBackParse(iptr, backParse);
    }

    public PairMatch getPairMatch(int lineNo, int linePos) {
        checkActive();
        LineRegion[] lrArray = getLineRegions(iptr, lineNo);
        if (lrArray.length == 0)
            return null;

        LineRegion pair = null;
        for (int idx = 0; idx < lrArray.length; idx++) {
            LineRegion l1 = lrArray[idx];
            if (l1.region == null)
                continue;
            if ((l1.region.hasParent(defPairStart) || l1.region
                    .hasParent(defPairEnd))
                    && linePos >= l1.start && linePos <= l1.end)
                pair = l1;
        }
        ;
        if (pair != null) {
            PairMatch pm = new PairMatch(pair, null, lineNo, -1, -1, false);
            if (pair.region.hasParent(defPairStart)) {
                pm.pairBalance = 1;
                pm.topPosition = true;
            }
            ;
            return pm;
        }
        return null;
    }

    int getLastVisibleLine() {
        checkActive();
        int r1 = (wStart + wSize);
        int r2 = lineCount;
        return ((r1 > r2) ? r2 : r1) - 1;
    }

    void searchPair(PairMatch pm, int start_line, int end_line) {
        checkActive();
        LineRegion pair = pm.start;
        int lno = pm.sline;
        LineRegion[] slr = getLineRegions(lno);
        int li = 0;
        while (li < slr.length && slr[li] != pair)
            li++;
        if (li == slr.length)
            return;

        while (true) {
            if (pm.pairBalance > 0) {
                li++;
                while (li == slr.length) {
                    lno++;
                    if (lno > end_line)
                        break;
                    slr = getLineRegions(lno);
                    li = 0;
                }
                ;
                if (lno > end_line)
                    break;
                pair = slr[li];
            } else {
                if (li == 0) {
                    lno--;
                    if (lno < start_line)
                        break;
                    slr = getLineRegions(lno);
                    li = slr.length;
                }
                ;
                if (lno < start_line)
                    break;
                li--;
                pair = slr[li];
            }
            ;
            if (pair.region == null)
                continue;
            if (pair.region.hasParent(defPairStart))
                pm.pairBalance++;
            if (pair.region.hasParent(defPairEnd))
                pm.pairBalance--;
            if (pm.pairBalance == 0)
                break;
        }
        ;
        if (pm.pairBalance == 0) {
            pm.eline = lno;
            pm.end = pair;
        }
        ;
    };

    public void searchLocalPair(PairMatch pm) {
        checkActive();
        int end_line = getLastVisibleLine();
        searchPair(pm, wStart, end_line);
    }

    public void searchGlobalPair(PairMatch pm) {
        checkActive();
        searchPair(pm, 0, lineCount - 1);
    }

    public LineRegion[] getLineRegions(int lno) {
        checkActive();
        Logger.trace("BaseEditorNative", "getLineRegions:"+lno);
        return getLineRegions(iptr, lno);
    }

    public void validate(int lno) {
        checkActive();
        validate(iptr, lno);
    }

    public void idleJob(int time) {
        checkActive();
        idleJob(iptr, time);
    }

    public void modifyEvent(int topLine) {
        checkActive();
        for (int idx = editorListeners.size()-1; idx >= 0; idx--) {
            ((EditorListener)editorListeners.elementAt(idx)).modifyEvent(topLine);
        }
        modifyEvent(iptr, topLine);
    }

    public void modifyLineEvent(int line) {
        checkActive();
        modifyLineEvent(iptr, line);
    }

    public void visibleTextEvent(int wStart, int wSize) {
        checkActive();
        visibleTextEvent(iptr, wStart, wSize);
        this.wStart = wStart;
        this.wSize = wSize;
    }

    public void lineCountEvent(int newLineCount) {
        checkActive();
        lineCountEvent(iptr, newLineCount);
        lineCount = newLineCount;
    }

    native long init(ParserFactory pf, LineSource lineSource);

    native void finalize(long iptr);

    native void setRegionCompact(long iptr, boolean compact);

    native void setRegionMapper(long iptr, RegionMapper regionMapper);

    native void setRegionMapper(long iptr, String hrdClass, String hrdName);

    native void addRegionHandler(long iptr, RegionHandler rh, Region filter);

    native void removeRegionHandler(long iptr, RegionHandler rh);

    native void setFileType(long iptr, FileType typename);

    native FileType getFileType(long iptr);

    native FileType chooseFileType(long iptr, String fname);

    native void setBackParse(long iptr, int backParse);

    native RegionDefine getBackground(long iptr);

    native RegionDefine getVertCross(long iptr);

    native RegionDefine getHorzCross(long iptr);

    native LineRegion[] getLineRegions(long iptr, int lno);

    native void validate(long iptr, int lno);

    native void idleJob(long iptr, int time);

    native void modifyEvent(long iptr, int topLine);

    native void modifyLineEvent(long iptr, int line);

    native void visibleTextEvent(long iptr, int wStart, int wSize);

    native void lineCountEvent(long iptr, int newLineCount);

};
/*******************************************************************************
 * ***** BEGIN LICENSE BLOCK Version: MPL 1.1/GPL 2.0/LGPL 2.1
 * 
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with the
 * License. You may obtain a copy of the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for
 * the specific language governing rights and limitations under the License.
 * 
 * The Original Code is the Colorer Library.
 * 
 * The Initial Developer of the Original Code is Cail Lomecb <cail@nm.ru>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2003 the
 * Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or the
 * GNU Lesser General Public License Version 2.1 or later (the "LGPL"), in which
 * case the provisions of the GPL or the LGPL are applicable instead of those
 * above. If you wish to allow use of your version of this file only under the
 * terms of either the GPL or the LGPL, and not to allow others to use your
 * version of this file under the terms of the MPL, indicate your decision by
 * deleting the provisions above and replace them with the notice and other
 * provisions required by the GPL or the LGPL. If you do not delete the
 * provisions above, a recipient may use your version of this file under the
 * terms of any one of the MPL, the GPL or the LGPL.
 * 
 * ***** END LICENSE BLOCK *****
 */