package net.sf.colorer.eclipse;

import net.sf.colorer.swt.dialog.ResourceManager;

import org.eclipse.jface.preference.*;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.*;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;

public class FileTypePreferencePage implements IWorkbenchPreferencePage{

  private Composite root;

  public FileTypePreferencePage(){
    //super(, FieldEditorPreferencePage.GRID);
    //setPreferenceStore(EclipsecolorerPlugin.getDefault().getPreferenceStore());
  }

  public void init(IWorkbench iworkbench){}

  Composite createComposite(Composite parent){
    Composite composite = new Composite(parent, SWT.NONE);
    composite.setLayout(new GridLayout(2, false));
    {
      final Tree tree = new Tree(composite, SWT.NONE);
      tree.setLayoutData(new GridData(GridData.FILL_BOTH));
    }
    {
      final Composite comp = new Composite(composite, SWT.NONE);
      comp.setLayout(new GridLayout(2, true));
      comp.setLayoutData(new GridData(GridData.FILL_BOTH));
    }
    //  DESIGNER: Add controls before this line.    
    return composite;
  }


  public void createFieldEditors(){
    //Composite p = getFieldEditorParent();
    
  }

  public Point computeSize() {
    return new Point(300, 500);
  }
  
  public void createControl(Composite parent) {
    root = createComposite(parent);
  }

  public boolean isValid() {
    return true;
  }

  public boolean okToLeave() {
    return true;
  }

  public boolean performCancel() {
    return true;
  }

  public boolean performOk() {
    return true;
  }

  public void setContainer(IPreferencePageContainer preferencePageContainer) {
    // TODO Auto-generated method stub
  }

  public void setSize(Point size) {
    // TODO Auto-generated method stub
  }

  public void dispose() {
    // TODO Auto-generated method stub
  }

  public Control getControl() {
    return root;
  }

  public String getDescription() {
    // TODO Auto-generated method stub
    return "xxx";
  }

  public String getErrorMessage() {
    return null;
  }

  public Image getImage() {
    // TODO Auto-generated method stub
    return null;
  }

  public String getMessage() {
    // TODO Auto-generated method stub
    return null;
  }

  public String getTitle() {
    return Messages.get("prefs.filetypes.title");
  }

  public void performHelp() {
    // TODO Auto-generated method stub
  }

  public void setDescription(String description) {
    // TODO Auto-generated method stub
  }

  public void setImageDescriptor(ImageDescriptor image) {
    // TODO Auto-generated method stub
  }

  public void setTitle(String title) {
    // TODO Auto-generated method stub
  }

  public void setVisible(boolean visible) {
    // TODO Auto-generated method stub
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
