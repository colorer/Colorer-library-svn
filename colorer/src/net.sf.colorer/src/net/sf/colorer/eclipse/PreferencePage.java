package net.sf.colorer.eclipse;

import java.util.Enumeration;
import java.util.Vector;

import net.sf.colorer.ParserFactory;

import org.eclipse.jface.preference.*;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;
import org.eclipse.ui.texteditor.WorkbenchChainedTextFontFieldEditor;

public class PreferencePage extends FieldEditorPreferencePage
    implements IWorkbenchPreferencePage{

  public final static String TEXT_FONT = "TEXT_FONT";
  public final static String SPACES_FOR_TABS = "SPACES_FOR_TABS";
  public final static String TAB_WIDTH = "TAB_WIDTH";
  public final static String WORD_WRAP = "WORD_WRAP";
  public final static String FULL_BACK = "FULL_BACK";
  public final static String VERT_CROSS = "VERT_CROSS";
  public final static String HORZ_CROSS = "HORZ_CROSS";
  public final static String HRD_SET = "HRD_SET";
  public final static String USE_BACK = "USE_BACK";
  public final static String PAIRS_MATCH = "PAIRS_MATCH";

  public final static String RELOAD_HRC = "RELOAD_HRC";
  
  public PreferencePage(){
    super(Messages.getString("prefs.title"), FieldEditorPreferencePage.GRID);
    setPreferenceStore(EclipsecolorerPlugin.getDefault().getPreferenceStore());
  }

  public void init(IWorkbench iworkbench){}

  public void createFieldEditors(){
    Composite p = getFieldEditorParent();
    
    addField(new WorkbenchChainedTextFontFieldEditor(TEXT_FONT, Messages.getString(TEXT_FONT), p));
    addField(new BooleanFieldEditor(SPACES_FOR_TABS, Messages.getString(SPACES_FOR_TABS), p));
    IntegerFieldEditor tabsnum = new IntegerFieldEditor(TAB_WIDTH, Messages.getString(TAB_WIDTH), p);
    tabsnum.setValidRange(1, 16);
    addField(tabsnum);
    addField(new BooleanFieldEditor(WORD_WRAP, Messages.getString(WORD_WRAP), p));

    addField(new BooleanFieldEditor(FULL_BACK, Messages.getString(FULL_BACK), p));
    addField(new BooleanFieldEditor(HORZ_CROSS, Messages.getString(HORZ_CROSS), p));
    //addField(new BooleanFieldEditor(VERT_CROSS, Messages.getString(VERT_CROSS), p));
    
    String[][] arrPairs = new String[4][2];
    arrPairs[0][0] = Messages.getString("PAIRS_NO");
    arrPairs[0][1] = "PAIRS_NO";
    arrPairs[1][0] = Messages.getString("PAIRS_XOR");
    arrPairs[1][1] = "PAIRS_XOR";
    arrPairs[2][0] = Messages.getString("PAIRS_OUTLINE");
    arrPairs[2][1] = "PAIRS_OUTLINE";
	arrPairs[3][0] = Messages.getString("PAIRS_OUTLINE2");
	arrPairs[3][1] = "PAIRS_OUTLINE2";
    addField(new RadioGroupFieldEditor(PAIRS_MATCH, Messages.getString(PAIRS_MATCH), 1, arrPairs, p));
    
    ParserFactory pf = EclipsecolorerPlugin.getDefault().getParserFactory();
    Vector radios = new Vector();
    for(Enumeration hrds = pf.enumerateHRDInstances("rgb"); hrds.hasMoreElements();){
      String hrd_name = (String)hrds.nextElement();
      String hrd_descr = pf.getHRDescription("rgb", hrd_name);
      radios.addElement(new String[]{ hrd_name, hrd_descr});
    }
    String[][] sradios = new String[radios.size()][2];
    for(int idx = 0; idx < radios.size(); idx++){
      sradios[idx][0] = ((String[])radios.elementAt(idx))[1];
      sradios[idx][1] = ((String[])radios.elementAt(idx))[0];
    }
    addField(new RadioGroupFieldEditor(HRD_SET, Messages.getString(HRD_SET), 1, sradios, p));
    addField(new BooleanFieldEditor(USE_BACK, Messages.getString(USE_BACK), p));
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