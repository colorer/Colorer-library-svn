package net.sf.colorer.eclipse.outline;

import java.util.Stack;
import java.util.Vector;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.model.IWorkbenchAdapter;

import net.sf.colorer.Region;
import net.sf.colorer.RegionHandler;
import net.sf.colorer.Scheme;
import net.sf.colorer.editor.OutlineItem;
import net.sf.colorer.editor.OutlineListener;
import net.sf.colorer.impl.Logger;
import net.sf.colorer.swt.TextColorer;


/**
 * Object used to store parse tree and showup it in Outline view 
 */
public class ParseTreeOutliner implements IWorkbenchOutlineSource, RegionHandler, IWorkbenchAdapter, IAdaptable {

    Vector listeners = new Vector();
    Vector regionsList = new Vector();
    
    Vector cList = regionsList;
    
    boolean changed = false;
    int modifiedLine = -1;
    int curLevel;
    

    public Region getFilter() {
        return null;
    }
    
    public void clear() {
        regionsList.clear();
    }

    public void startParsing(int lno) {
        curLevel = 0;
        cList = regionsList;
    }

    public void endParsing(int lno) {
        curLevel = 0;
        if (changed) {
            notifyUpdate();
            changed = false;
        }
    }

    public void clearLine(int lno, String line) {
    }

    public void addRegion(int lno, String line, int sx, int ex, Region region) {
        if (Logger.TRACE) {
            Logger.trace("ParseTree", sx+"-"+ex+": "+line);
        }

        cList.addElement(new OutlineElement(this, lno, sx, ex-sx, curLevel, null, region));
        changed = true;
    }

    Stack schemeStack = new Stack();
    
    public void enterScheme(int lno, String line, int sx, int ex, Region region, String scheme) {
        curLevel++;
        Vector elements = new Vector();
        OutlineSchemeElement scheme_el = new OutlineSchemeElement(this, lno, sx, ex-sx, curLevel, scheme, region, elements);
        cList.addElement(scheme_el);
        schemeStack.push(cList);
        cList = elements;
    }
    public void leaveScheme(int lno, String line, int sx, int ex, Region region, String scheme) {
        curLevel--;
        cList = (Vector)schemeStack.pop();
    }

    public void modifyEvent(int topLine) {
        clear();
        changed = true;
    }

    public Object[] getChildren(Object o) {
        if (o == this) {
            return regionsList.toArray();
        }
        if (o instanceof OutlineSchemeElement) { 
            return ((OutlineSchemeElement)o).elements.toArray();
        }
        return new Object[] {};
    }

    public ImageDescriptor getImageDescriptor(Object object) {
        return null;
    }

    public String getLabel(Object o) {
        if (o instanceof OutlineSchemeElement) {
            StringBuffer scheme = ((OutlineSchemeElement)o).token;
            return "scheme: " + scheme;
        }
        return ((OutlineItem)o).region.getName();
    }

    public Object getParent(Object o) {
        return null;
    }

    public Object getAdapter(Class adapter) {
        if (adapter == IWorkbenchAdapter.class){
            return this;
        }
        return null;
    }

    public void setHierarchy(boolean hierarchy) {
    }
    public void setSorting(boolean sorting) {
    }

    public void attachOutliner(TextColorer editor) {
        editor.getBaseEditor().addRegionHandler(this, null);
    }
    public void detachOutliner(TextColorer editor) {
        editor.getBaseEditor().removeRegionHandler(this);
    }

    public void addListener(OutlineListener listener) {
        listeners.addElement(listener);
    }

    public void removeListener(OutlineListener listener) {
        listeners.removeElement(listener);
    }
    
    protected void notifyUpdate(){
        for (int idx = 0; idx < listeners.size(); idx++)
            ((OutlineListener) listeners.elementAt(idx)).notifyUpdate();
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