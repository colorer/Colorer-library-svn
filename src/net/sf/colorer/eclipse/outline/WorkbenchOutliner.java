package net.sf.colorer.eclipse.outline;

import java.io.File;
import java.util.Hashtable;
import java.util.Vector;

import net.sf.colorer.Region;
import net.sf.colorer.RegionHandler;
import net.sf.colorer.eclipse.ImageStore;
import net.sf.colorer.editor.OutlineItem;
import net.sf.colorer.editor.OutlineListener;
import net.sf.colorer.editor.Outliner;
import net.sf.colorer.impl.Logger;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.model.IWorkbenchAdapter;

/**
 * Default outliner, used to filter parse stream for the specified
 * HRC Region.
 */
public class WorkbenchOutliner extends Outliner
        implements IWorkbenchAdapter, IWorkbenchOutlineSource
{
    Hashtable iconsHash = new Hashtable();
    Vector listeners = new Vector();
    boolean changed = true;
    boolean hierarchy = true;

    public WorkbenchOutliner(Region filter) {
        super(filter);
    }

    public void addListener(OutlineListener listener) {
        listeners.addElement(listener);
    }

    public void removeListener(OutlineListener listener) {
        listeners.removeElement(listener);
    }

    public void setHierarchy(boolean hierarchy) {
        this.hierarchy = hierarchy;
        notifyUpdate();
    }
    public void setSorting(boolean sorting) {
    }
    
    public RegionHandler getRegionHandler(){
        return this;
    }

    public OutlineItem createItem(int lno, int sx, int curLevel, String itemLabel, Region region) {
        return new OutlineElement(this, lno, sx, curLevel, itemLabel, region);
    }

    public void startParsing(int lno) {
        Logger.trace("WorkbenchOutliner", "startParsing("+lno+"):"+this);
        super.startParsing(lno);
        changed = true;
    }

    public void endParsing(int lno) {
        Logger.trace("WorkbenchOutliner", "endParsing("+lno+"):"+this);
        super.endParsing(lno);
        if (changed) {
            notifyUpdate();
        }
    };
    
    void notifyUpdate(){
        for (int idx = 0; idx < listeners.size(); idx++)
            ((OutlineListener) listeners.elementAt(idx)).notifyUpdate();
        changed = false;
    }

    // represents root of Outline structure
    public Object getAdapter(Class adapter) {
        if (adapter == IWorkbenchAdapter.class){
            return this;
        }
        return null;
    }

    public Object[] getChildren(Object object) {
        Vector elements = new Vector();

        if (!hierarchy) {
            if (object == this) {
                for (int idx = 0; idx < itemCount(); idx++)
                    elements.addElement(getItem(idx));
            }
            ;
        } else {
            if (object == this) {
                int flevel = 0x100000;
                for (int idx = 0; idx < itemCount(); idx++) {
                    if (flevel > getItem(idx).level)
                        flevel = getItem(idx).level;
                    if (getItem(idx).level > flevel)
                        continue;
                    elements.addElement(getItem(idx));
                }
            } else if (object instanceof OutlineElement) {
                OutlineElement el = (OutlineElement) object;
                int idx = outline.indexOf(el);
                if (idx > -1) {
                    int flevel = 0x100000;
                    for (idx++; idx < itemCount(); idx++) {
                        if (getItem(idx).level <= el.level)
                            break;
                        if (flevel > getItem(idx).level)
                            flevel = getItem(idx).level;
                        if (getItem(idx).level > flevel)
                            continue;
                        elements.addElement(getItem(idx));
                    }
                }
            }
        }
        return elements.toArray();
    }

    public ImageDescriptor getImageDescriptor(Object object) {
        String iconName = null;
        if (object instanceof OutlineElement) {
            OutlineElement el = (OutlineElement) object;
            iconName = searchIcons(el.region);
        }
        if (iconName == null)
            return null;
        return ImageStore.getID(iconName);
    }

    String searchIcons(Region region) {
        if (region == null)
            return null;
        if (iconsHash.get(region.getName()) != null)
            return (String) iconsHash.get(region.getName());
        for (; region != null; region = region.getParent()) {
            String iconName = "outline" + File.separator
                    + region.getName().replace(':', File.separatorChar);
            ImageDescriptor id = ImageStore.getID(iconName);
            if (id != null) {
                iconsHash.put(region.getName(), iconName);
                return iconName;
            }
        }
        return "outline" + File.separator + "def" + File.separator + "Outlined";
    }

    public String getLabel(Object object) {
        if (object instanceof OutlineElement) {
            OutlineElement el = (OutlineElement) object;
            return el.token.toString();
        }
        return null;
    }

    public Object getParent(Object o) {
        return null;
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