/*
 * Created on 05.02.2006
 *
 */
package net.sf.colorer.eclipse.ftpp;

import net.sf.colorer.FileType;
import net.sf.colorer.eclipse.ColorerPlugin;
import net.sf.colorer.eclipse.ImageStore;
import net.sf.colorer.eclipse.Messages;

import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;

/**
 * Labels for specific parameter values
 *
 * @author Igor Russkih
 */
class TypeParametersLabelProvider extends LabelProvider implements ITableLabelProvider{
    Image grImage = ImageStore.EDITOR_CUR_GROUP.createImage();
    Image grImageDis = ImageStore.EDITOR_GROUP.createImage();
    Image hrdImage = ImageStore.EDITOR_UPDATEHRC_A.createImage();

    TypeParametersContentProvider contentProvider;

    public TypeParametersLabelProvider(TypeParametersContentProvider cp){
        contentProvider = cp;
    }

    public Image getColumnImage(Object element, int columnIndex) {
        if (TypeParametersContentProvider.HRD_SIGNATURE.equals(element)) {
            return hrdImage;
        }
        String value = contentProvider.type.getParameterValue((String)element);
        if ("true".equals(value)) {
            return grImage;
        }else {
            return grImageDis;
        }
    }

    public String getColumnText(Object element, int columnIndex) {
        if (contentProvider == null) {
            return null;
        }
        FileType type = contentProvider.type;
        /* Value of HRD scheme */
        if (TypeParametersContentProvider.HRD_SIGNATURE.equals(element)) {
            if (columnIndex == 0) {
                return Messages.get("ftpp.hrd_set");
            }else {
                String hrd = contentProvider.getAssignedHRD();
                String hrd_descr = null;
                if (hrd == null) hrd_descr = Messages.get("ftpp.default_hrd");
                else hrd_descr = ColorerPlugin.getDefaultPF().getHRDescription("rgb", hrd); 
                return hrd_descr;
            }
        }
        /* or HRC parameter value */
        if (columnIndex == 0) {
            return type.getParameterDescription((String)element);
        }else {
            return type.getParameterValue((String)element);
        }
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
 * Igor Russkih <irusskih at gmail.com>.
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
