package net.sf.colorer.editor;

import java.util.Vector;

import net.sf.colorer.Region;
import net.sf.colorer.RegionHandler;
import net.sf.colorer.Scheme;

public class Outliner implements RegionHandler{

protected Vector outline = new Vector();
Region searchRegion = null;
int curLevel = 0;
boolean lineIsEmpty = true;

public Outliner(Region searchRegion){
  this.searchRegion = searchRegion;
};

public OutlineItem getItem(int idx){
  return (OutlineItem)outline.elementAt(idx);
};

public int itemCount(){
  return outline.size();
};
public Region getFilter(){
  return searchRegion;
}

static int manageTree(Vector treeStack, int newLevel){
  while(treeStack.size() > 0 && newLevel < ((Integer)treeStack.lastElement()).intValue())
    treeStack.removeElementAt(treeStack.size()-1);
  if (treeStack.size() == 0 || newLevel > ((Integer)treeStack.lastElement()).intValue()){
    treeStack.addElement(new Integer(newLevel));
    return treeStack.size()-1;
  };
  if (newLevel == ((Integer)treeStack.lastElement()).intValue()) return treeStack.size()-1;
  return 0;
};


public boolean isOutlined(Region region){
  return region.hasParent(searchRegion);
};
public OutlineItem createItem(int lno, int sx, int curLevel, String itemLabel, Region region){
  return new OutlineItem(lno, sx, curLevel, itemLabel, region);
};

public void startParsing(int lno){
  int new_size;
  for(new_size = outline.size()-1; new_size >= 0; new_size--){
    if (((OutlineItem)outline.elementAt(new_size)).lno < lno) break;
  };
  outline.setSize(new_size+1);
  curLevel = 0;
};
public void endParsing(int lno){
  curLevel = 0;
};
public void clearLine(int lno, String line){
  lineIsEmpty = true;
};
public void addRegion(int lno, String line, int sx, int ex, Region region){
  if (!isOutlined(region)) return;
  String itemLabel = null;
  if (line != null) itemLabel = line.substring(sx, ex);

  if (lineIsEmpty){
    outline.addElement(createItem(lno, sx, curLevel, itemLabel, region));
  }else{
    OutlineItem thisItem = (OutlineItem)outline.lastElement();
    if (itemLabel != null && thisItem.token != null && thisItem.lno == lno){
	  if (itemLabel.length() > 1) thisItem.token.append(" ");
      thisItem.token.append(itemLabel);
    }
  };
  lineIsEmpty = false;
};
public void enterScheme(int lno, String line, int sx, int ex, Region region, Scheme scheme){
  curLevel++;
};
public void leaveScheme(int lno, String line, int sx, int ex, Region region, Scheme scheme){
  curLevel--;
};

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
