package net.sf.colorer.handlers;

import net.sf.colorer.Region;

public class RegionMapper{

    private long iptr;
    
    private RegionMapper(long iptr)
    {
        this.iptr = iptr;
    }
    
    /**
     * Searches mapped region define value.
     * 
     * @return Region define, associated with passed
     * region parameter, or null if nothing found
     */
    RegionDefine getRegionDefine(Region region) {
        return getRegionDefine(iptr, region);
    }
    
    private native RegionDefine getRegionDefine(long iptr, Region region);

    /**
     * Searches mapped region define value with qualified name
     */
    RegionDefine getRegionDefine(String name) {
        return getRegionDefine(iptr, name);
    }
    private native RegionDefine getRegionDefine(long iptr, String name);

    /**
     * Static factory method to retrieve RegionMapper object instance,
     * associated with passed HRD class and name.
     * 
     * @param hrdClass Class of HRD descriptor
     * @param hrdName Name of HRD descriptor
     * @return Class instance
     */
    public static RegionMapper getRegionMapper(String hrdClass, String hrdName){
        long iptr = getRegionMapper0(hrdClass, hrdName);
        return new RegionMapper(iptr);
    }
    static native long getRegionMapper0(String hrdClass, String hrdName);
    
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