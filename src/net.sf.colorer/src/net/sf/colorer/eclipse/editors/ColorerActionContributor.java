package net.sf.colorer.eclipse.editors;

import java.util.Enumeration;
import java.util.Hashtable;

import net.sf.colorer.*;
import net.sf.colorer.eclipse.EclipsecolorerPlugin;
import net.sf.colorer.eclipse.HRCMessages;
import net.sf.colorer.eclipse.ImageStore;
import net.sf.colorer.eclipse.Messages;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IMenuCreator;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.ui.*;
import org.eclipse.ui.editors.text.TextEditorActionContributor;

public class ColorerActionContributor extends TextEditorActionContributor{
  ColorerEditor activeEditor = null;

  UpdateAction hrcupdateAction;
  FileTypeActionMenu filetypeAction;
  public PairMatchAction pairMatchAction;
  public PairSelectAction pairSelectAction;
  public PairSelectContentAction pairSelectContentAction;

  class PairMatchAction extends Action{
    PairMatchAction(){
        super(Messages.get("pair.find"), ImageStore.EDITOR_PAIR_MATCH);
      setActionDefinitionId("net.sf.colorer.eclipse.editors.pairmatch");
          setToolTipText(Messages.get("pair.find.tooltip"));
    }
    public void run(){
      activeEditor.matchPair();
    }
  }
  class PairSelectAction extends Action{
    PairSelectAction(){
        super(Messages.get("pair.select"), ImageStore.EDITOR_PAIR_SELECT);
      setActionDefinitionId("net.sf.colorer.eclipse.editors.pairselect");
          setToolTipText(Messages.get("pair.select.tooltip"));
    }
    public void run(){
      activeEditor.selectPair();
    }
  }
  class PairSelectContentAction extends Action{
    PairSelectContentAction(){
        super(Messages.get("pair.selectcontent"), ImageStore.EDITOR_PAIR_SELECTCONTENT);
      setActionDefinitionId("net.sf.colorer.eclipse.editors.pairselectcontent");
          setToolTipText(Messages.get("pair.selectcontent.tooltip"));
    }
    public void run(){
      activeEditor.selectContentPair();
    }
  }

  class UpdateAction extends Action {
    public UpdateAction(String label) {
      super(label);
    }
    public void run() {
      if (activeEditor != null && activeEditor instanceof ColorerEditor){
        EclipsecolorerPlugin.getDefault().reloadParserFactory();
      }
    }
  }

  class FileTypeActionMenu extends Action implements IMenuCreator{
    class FileTypeAction extends Action{
      FileType ftype;
      FileTypeAction(FileType type){ super(type.getDescription()); ftype = type; }
      public void run(){
        if (activeEditor != null && activeEditor instanceof ColorerEditor){
          ((ColorerEditor)activeEditor).setFileType(ftype);
        }
      }
    }
    private Menu filetypeList = null;
    public FileTypeActionMenu(String label) {
      super(label);
      setMenuCreator(this);
    }
    public void run() {
      ((ColorerEditor)activeEditor).chooseFileType();
    };

    Menu getMenuTree(Hashtable h, Menu root, String groupname){
			Menu mgroup = (Menu)h.get(groupname);
			String origname = groupname;
			if (mgroup == null){
			  int idx = groupname.lastIndexOf('.');
			  if (idx != -1){
			  	root = getMenuTree(h, root, groupname.substring(0, idx));
			  	groupname = groupname.substring(idx+1, groupname.length());
			  }
			  MenuItem mitem = new MenuItem(root, SWT.CASCADE);
			  mitem.setText(HRCMessages.get(origname, groupname));
			  mgroup = new Menu(mitem);
			  h.put(origname, mgroup);
			  mitem.setMenu(mgroup);
				mgroup.getParentItem().setImage(ImageStore.EDITOR_GROUP.createImage());
			}
			return mgroup;
    }

    public Menu getMenu(Control parent){
      if (filetypeList != null) filetypeList.dispose();
      filetypeList = new Menu(parent);
      ActionContributionItem item;
      ParserFactory pf = EclipsecolorerPlugin.getDefault().getParserFactory();
      Hashtable groups = new Hashtable();
      for(Enumeration e = pf.getHRCParser().enumerateFileTypes(); e.hasMoreElements();){
        FileType ftype = (FileType)e.nextElement();
        //String ftgroup = pf.getFileTypeGroup(ftype);
        //String ftdescription = pf.getFileTypeDescription(ftype);

        Menu mgroup = getMenuTree(groups, filetypeList, ftype.getGroup());
        FileTypeAction ft_action = new FileTypeAction(ftype);
        if (((ColorerEditor)activeEditor).getFileType().equals(ftype)){
          mgroup.getParentItem().setImage(ImageStore.EDITOR_CUR_GROUP.createImage());
          if (mgroup.getParentItem().getParent().getParentItem() != null)
            mgroup.getParentItem().getParent().getParentItem().setImage(ImageStore.EDITOR_CUR_GROUP.createImage());
          ft_action.setImageDescriptor(ImageStore.EDITOR_CUR_FILETYPE);
        }
        item = new ActionContributionItem(ft_action);
        item.fill(mgroup, -1);
      }
      return filetypeList;
    }

    public Menu getMenu(Menu parent){
      return null;
    }
    public void dispose(){
      if (filetypeList != null) filetypeList.dispose();
    };
  }



public ColorerActionContributor() {

  hrcupdateAction = new UpdateAction(Messages.get("editor.hrcupdate"));
  hrcupdateAction.setToolTipText(Messages.get("editor.hrcupdate.tooltip"));
  hrcupdateAction.setImageDescriptor(ImageStore.EDITOR_UPDATEHRC);
  hrcupdateAction.setHoverImageDescriptor(ImageStore.EDITOR_UPDATEHRC_A);
  hrcupdateAction.setActionDefinitionId("net.sf.colorer.eclipse.editors.hrcupdate");

  filetypeAction = new FileTypeActionMenu(Messages.get("editor.filetype"));
  filetypeAction.setToolTipText(Messages.get("editor.filetype.tooltip"));
  filetypeAction.setImageDescriptor(ImageStore.EDITOR_FILETYPE);
  filetypeAction.setHoverImageDescriptor(ImageStore.EDITOR_FILETYPE_A);
  filetypeAction.setActionDefinitionId("net.sf.colorer.eclipse.editors.choosetype");

  pairMatchAction = new PairMatchAction();
  pairSelectAction = new PairSelectAction();
  pairSelectContentAction = new PairSelectContentAction();
}

public void contributeToToolBar(IToolBarManager toolBarManager) {
    super.contributeToToolBar(toolBarManager);

    toolBarManager.add(new Separator("net.sf.colorer.eclipse.editor.Separator"));
    toolBarManager.add(hrcupdateAction);
  toolBarManager.add(filetypeAction);
    toolBarManager.add(new Separator("net.sf.colorer.eclipse.editor.Separator1"));
  toolBarManager.add(pairMatchAction);
  toolBarManager.add(pairSelectAction);
  toolBarManager.add(pairSelectContentAction);
}

public void dispose() {
}

public void init(IActionBars bars, IWorkbenchPage page) {
    super.init(bars, page);
}

public void setActiveEditor(IEditorPart editor) {
    super.setActiveEditor(editor);
  activeEditor = (ColorerEditor)editor;

  IKeyBindingService kbs = editor.getEditorSite().getKeyBindingService();
  kbs.registerAction(hrcupdateAction);
  kbs.registerAction(filetypeAction);
  kbs.registerAction(pairMatchAction);
  kbs.registerAction(pairSelectAction);
  kbs.registerAction(pairSelectContentAction);
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