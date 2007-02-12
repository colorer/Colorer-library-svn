package net.sf.colorer.editor;

import java.util.Vector;

import net.sf.colorer.Region;
import net.sf.colorer.RegionHandler;

/**
 * 
 */
public class FoldingBuilder {

    public class FoldingElement {

        int s_line;
        int s_offset;
        String scheme;

        FoldingElement(int line, int offset, String scheme){
            this.scheme = scheme;
            s_line = line;
            s_offset = offset;
        }
    }

	public class InternalRegionHandler implements RegionHandler, EditorListener {
        
        Vector schemeVector = new Vector();
        private int ignoreLevel;
        
        public void addRegion(int lno, String line, int sx, int ex, Region region) {
            // TODO Auto-generated method stub

        }

        public void clearLine(int lno, String line) {
            // TODO Auto-generated method stub

        }

        public void startParsing(int lno) {
            schemeVector.setSize(0);
            fReciever.notifyInvalidate(lno);
        }
        public void endParsing(int lno) {
        }

        public void enterScheme(int lno, String line, int sx, int ex,
                Region region, String scheme)
        {
            if (schemeVector.size() > 0) {
                FoldingElement last = (FoldingElement)schemeVector.lastElement();
                if (last.s_line == lno) {
                    ignoreLevel++;
                    return;
                }
            }
            schemeVector.addElement(new FoldingElement(lno, sx, scheme));

        }

        /** 
         * @see net.sf.colorer.RegionHandler#leaveScheme(int, java.lang.String, int, int, net.sf.colorer.Region, java.lang.String)
         */
        public void leaveScheme(int lno, String line, int sx, int ex,
                Region region, String scheme)
        {
            if (ignoreLevel > 0) {
                ignoreLevel--;
                return;
            }

            FoldingElement last = (FoldingElement)schemeVector.lastElement();
            
            if (last.s_line < lno){
                fReciever.notifyFoldingItem(last.s_line, last.s_offset, lno, ex, last.scheme);
            }
            
            schemeVector.setSize(schemeVector.size()-1);
        }

        //----------------------------------------------
        
        public void modifyEvent(int topLine) {
            // TODO Auto-generated method stub
            
        }
    }

	private BaseEditor fBaseEditor;
    private InternalRegionHandler fHandler = new InternalRegionHandler();
    private IFoldingReciever fReciever;
    private int fThreshold = 1;
    
    
    public void getFoldingItems() {
        //
    }

    /**
     * Sets the folding items minimum separation.
     * 
     * @param linesThreshold In lines. By default threshold is 1, which
     * means two lines can both contain folding items.
     */
    public void setThreshold(int linesThreshold) {
        fThreshold = linesThreshold;
    }
    
    /**
     * Installs this builder over specified BaseEditor
     */
    public void install(BaseEditor baseEditor, IFoldingReciever reciever) {
        fBaseEditor = baseEditor;
        fReciever = reciever;
        fBaseEditor.addRegionHandler(fHandler, null);
        fBaseEditor.addEditorListener(fHandler);
    }
    
    /**
     * Stops this builder, no folding structure is built anymore.
     */
    public void uninstall() {
        fBaseEditor.removeRegionHandler(fHandler);
        fBaseEditor.removeEditorListener(fHandler);
    }

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
 * The Original Code is the Colorer Library
 *
 * The Initial Developer of the Original Code is
 * Igor Russkih <irusskih at gmail dot com>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2007
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