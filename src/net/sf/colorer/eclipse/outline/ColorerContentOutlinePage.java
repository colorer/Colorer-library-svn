package net.sf.colorer.eclipse.outline;

import java.util.Enumeration;
import java.util.Vector;

import net.sf.colorer.eclipse.*;

import org.eclipse.jface.action.*;
import org.eclipse.jface.viewers.*;
import org.eclipse.ui.model.WorkbenchViewerSorter;
import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.widgets.*;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.contentoutline.ContentOutlinePage;

/**
 * Content outline page for the readme editor.
 */
public class ColorerContentOutlinePage extends ContentOutlinePage implements OutlineListener
{
  WorkbenchOutliner structureOutliner = null;
  WorkbenchOutliner errorsOutliner = null;
  WorkbenchOutliner activeOutliner = null;
  
  StructureModeAction structureModeAction = null;
  ErrorsModeAction errorsModeAction = null;

  Thread backgroundUpdater = null;
  boolean outlineModified = true;
  long prevTime = 0;
  int UPDATE_DELTA = 3000;

  class TreeAction extends Action{
    public TreeAction(){
      super(Messages.get("outline.tree"), ImageStore.getID("outline-tree"));
      setToolTipText(Messages.get("outline.tree.tooltip"));
      setHoverImageDescriptor(ImageStore.getID("outline-tree-hover"));
      setChecked(ColorerPlugin.getDefault().getPreferenceStore().getBoolean("Outline.Hierarchy"));
    };
    public void run(){
      setChecked(isChecked());
      ColorerPlugin.getDefault().getPreferenceStore().setValue("Outline.Hierarchy", isChecked());
      activeOutliner.setHierarchy(isChecked());
      update();
    }
  }

  class SortAction extends Action{
    private ViewerSorter sorter = new WorkbenchViewerSorter();
    public SortAction(){
      super(Messages.get("outline.sort"), ImageStore.getID("outline-sort"));
      setToolTipText(Messages.get("outline.sort.tooltip"));
      setHoverImageDescriptor(ImageStore.getID("outline-sort-hover"));
      setChecked(ColorerPlugin.getDefault().getPreferenceStore().getBoolean("Outline.Sort"));
      getTreeViewer().setSorter(isChecked() ? sorter : null);
    };
    public void run(){
      setChecked(isChecked());
      BusyIndicator.showWhile(getControl().getDisplay(), new Runnable() {
        public void run() {
          getTreeViewer().setSorter(isChecked() ? sorter : null);
        }
      });
      ColorerPlugin.getDefault().getPreferenceStore().setValue("Outline.Sort", isChecked());
    }
  }
  
  class StructureModeAction extends Action {
    StructureModeAction(){
      this.setText(Messages.get("outline.options.Structure"));
      this.setImageDescriptor(ImageStore.getID("outline-options-structure"));
      setChecked(activeOutliner == structureOutliner);      
    }
    public void run(){
      activeOutliner = structureOutliner;
      errorsModeAction.setChecked(false);
      setChecked(true);
      update();
    };
  }
  
  class ErrorsModeAction extends Action {
    ErrorsModeAction(){
      setText(Messages.get("outline.options.Errors"));
      setImageDescriptor(ImageStore.getID("outline-options-errors"));
      setChecked(activeOutliner == errorsOutliner);
    }
    public void run(){
      activeOutliner = errorsOutliner;
      structureModeAction.setChecked(false);
      setChecked(true);
      update();
    };
  }
  

/*	class ClickAction extends Action {
		public ClickAction() {
			super();
			getTreeViewer().addSelectionChangedListener(new ISelectionChangedListener() {
				public void selectionChanged(SelectionChangedEvent event) {
					setEnabled(!event.getSelection().isEmpty());
				}
			});
		}
		public void run() {
			MessageDialog.openInformation(null,
				Messages.get("Readme_Outline"),
				Messages.get("ReadmeOutlineActionExecuted"));
		}
	}
*/



  public ColorerContentOutlinePage() {
  	super();
  }
  
  public void attach(WorkbenchOutliner structure, WorkbenchOutliner errors){
    detach();
  	structureOutliner = structure;
  	errorsOutliner = errors;
  	activeOutliner = structureOutliner;

    activeOutliner.addListener(this);

    activeOutliner.setHierarchy(ColorerPlugin.getDefault().getPreferenceStore().getBoolean("Outline.Hierarchy"));
    
    notifyUpdate();
    backgroundUpdater = new Thread(){
      public void run(){
        while(true){
          try{ sleep(UPDATE_DELTA); }catch(InterruptedException e){ break; };
          if (Thread.currentThread() != backgroundUpdater) break;
          Display.getDefault().syncExec(new Runnable(){
            public void run(){
              updateIfChanged();
            }
          });
        };
      };
    };
    backgroundUpdater.start();
  }
  public void detach(){
    if (structureOutliner != null) structureOutliner.removeListener(this);
    structureOutliner = null;
    errorsOutliner = null;
    backgroundUpdater = null;
  };
  public void dispose(){
    detach();
    super.dispose();
  }
  
  
  void update(){
    if (getControl() == null) return;
    if (getControl().isDisposed()) return;
    getControl().setRedraw(false);
    int hpos = getTreeViewer().getTree().getHorizontalBar().getSelection();
    int vpos = getTreeViewer().getTree().getVerticalBar().getSelection();
    getTreeViewer().setInput(activeOutliner);
    getTreeViewer().expandAll();
    getControl().setRedraw(true);
    getTreeViewer().getTree().getHorizontalBar().setSelection(hpos);
    getTreeViewer().getTree().getVerticalBar().setSelection(vpos);
  }
  public void updateIfChanged(){
    if (outlineModified){
      update();
      outlineModified = false;
    };
  }
  public void notifyUpdate(){
    outlineModified = true;
    long cTime = System.currentTimeMillis();
    if (cTime - prevTime > UPDATE_DELTA && activeOutliner != null){
      updateIfChanged();
      prevTime = System.currentTimeMillis();
    };
  }
  
  Vector doubleClickListeners = new Vector();
  public void addDoubleClickListener(IDoubleClickListener dc){
    if (doubleClickListeners.indexOf(dc) == -1) doubleClickListeners.add(dc);
  }
  public void removeDoubleClickListener(IDoubleClickListener dc){
    doubleClickListeners.remove(dc);
  }
  
  public void createControl(Composite parent) {
  	super.createControl(parent);
  
  	TreeViewer viewer = getTreeViewer();
    viewer.addDoubleClickListener(new IDoubleClickListener(){
      public void doubleClick(DoubleClickEvent event) {
        Enumeration e = doubleClickListeners.elements();
        for(; e.hasMoreElements();)
          ((IDoubleClickListener)e.nextElement()).doubleClick(event);
      }
    });

  	/* Configure the context menu.
  	MenuManager menuMgr = new MenuManager("#PopupMenu");
  	menuMgr.add(new Separator(IWorkbenchActionConstants.MB_ADDITIONS));	
  	menuMgr.add(new Separator(IWorkbenchActionConstants.MB_ADDITIONS+"-end"));
  
  	Menu menu = menuMgr.createContextMenu(viewer.getTree());
  	viewer.getTree().setMenu(menu);
    */
    
    IToolBarManager toolBarManager = getSite().getActionBars().getToolBarManager();
    if (toolBarManager != null) { 
      toolBarManager.add(new TreeAction());
      toolBarManager.add(new SortAction());
    };
    
    IMenuManager menuManager = getSite().getActionBars().getMenuManager();
    if (menuManager != null){
      structureModeAction = new StructureModeAction();
      errorsModeAction = new ErrorsModeAction();
      menuManager.add(structureModeAction);
      menuManager.add(errorsModeAction);
    }
    
  	viewer.setContentProvider(new WorkbenchContentProvider());
  	viewer.setLabelProvider(new WorkbenchLabelProvider());
  	viewer.setInput(activeOutliner);
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