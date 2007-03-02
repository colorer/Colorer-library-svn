package net.sf.colorer.eclipse.editors;

import net.sf.colorer.FileType;
import net.sf.colorer.Group;
import net.sf.colorer.eclipse.ColorerPlugin;
import net.sf.colorer.eclipse.ImageStore;
import net.sf.colorer.eclipse.Messages;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IMenuCreator;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.text.source.projection.ProjectionViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.editors.text.IFoldingCommandIds;
import org.eclipse.ui.editors.text.TextEditorActionContributor;
import org.eclipse.ui.texteditor.TextOperationAction;

/**
 * Action definitions for editor UI commands.
 *
 */
public class ColorerActionContributor extends TextEditorActionContributor
{
    
    public final static String ACTION_ID_PAIRMATCH = "net.sf.colorer.eclipse.editors.PairMatchAction";
    public final static String ACTION_ID_PAIRSELECT = "net.sf.colorer.eclipse.editors.pairselect";
    public final static String ACTION_ID_PAIRSELECTCONTENT = "net.sf.colorer.eclipse.editors.pairselectcontent";
    
    ColorerEditor activeEditor = null;

    Action pairMatchAction = new PairMatchAction();
    Action pairSelectAction = new PairSelectAction();
    Action pairSelectContentAction = new PairSelectContentAction();

    Action cursorRegionAction = new Action() {
        public void run(){
            activeEditor.selectCursorRegion();
        }
    };

    class PairMatchAction extends Action {

        PairMatchAction() {
            super(Messages.get("pair.find"), ImageStore.EDITOR_PAIR_MATCH);
            setActionDefinitionId(ACTION_ID_PAIRMATCH);
            setToolTipText(Messages.get("pair.find.tooltip"));
        }

        public void run() {
            activeEditor.matchPair();
        }
    }

    class PairSelectAction extends Action {

        PairSelectAction() {
            super(Messages.get("pair.select"), ImageStore.EDITOR_PAIR_SELECT);
            setActionDefinitionId(ACTION_ID_PAIRSELECT);
            setToolTipText(Messages.get("pair.select.tooltip"));
        }

        public void run() {
            activeEditor.selectPair();
        }
    }

    class PairSelectContentAction extends Action {

        PairSelectContentAction() {
            super(Messages.get("pair.selectcontent"), ImageStore.EDITOR_PAIR_SELECTCONTENT);
            setActionDefinitionId(ACTION_ID_PAIRSELECTCONTENT);
            setToolTipText(Messages.get("pair.selectcontent.tooltip"));
        }

        public void run() {
            activeEditor.selectContentPair();
        }
    }

    Action hrcupdateAction = new Action() {
        public void run() {
            if (activeEditor != null) {
                ColorerPlugin.getDefault().reloadParserFactory();
            }
        }
    };

    FileTypeActionMenu filetypeAction;
    class FileTypeActionMenu extends Action implements IMenuCreator{

      class FileTypeAction extends Action {

            FileType ftype;

            FileTypeAction(FileType type) {
                super(type.getDescription());
                ftype = type;
            }

            public void run() {
                if (activeEditor != null) {
                    ((ColorerEditor) activeEditor).setFileType(ftype);
                }
            }
        }

        private Menu filetypeList = null;

        public FileTypeActionMenu(String label) {
            super(label);
            setMenuCreator(this);
        }

        public void run() {
            ((ColorerEditor) activeEditor).chooseFileType();
        }

        Menu fillMenuTree(Group group, Menu root, FileType cftype) {
            
            MenuItem mitem = new MenuItem(root, SWT.CASCADE);
            mitem.setText(group.getDescription());

            Menu mgroup = new Menu(mitem);
            mitem.setMenu(mgroup);
            mgroup.getParentItem().setImage(ImageStore.EDITOR_GROUP.createImage());

            FileType ftypes[] = group.getFileTypes();
            for(int idx = 0; idx < ftypes.length; idx++){
                ActionContributionItem item;
                FileTypeAction ft_action = new FileTypeAction(ftypes[idx]);
                if (cftype.equals(ftypes[idx])) {
                    mgroup.getParentItem().setImage(ImageStore.EDITOR_CUR_GROUP.createImage());
                    if (mgroup.getParentItem().getParent().getParentItem() != null) {
                        mgroup.getParentItem().getParent().getParentItem().setImage(
                                ImageStore.EDITOR_CUR_GROUP.createImage());
                    }
                    ft_action.setImageDescriptor(ImageStore.EDITOR_CUR_FILETYPE);
                }
                item = new ActionContributionItem(ft_action);
                item.fill(mgroup, -1);
            }

            for(int idx = 0; idx < group.getGroups().length; idx++){
                fillMenuTree(group.getGroups()[idx], mgroup, cftype);
            }
            
            return mgroup;
        }

        public Menu getMenu(Control parent) {
            if (filetypeList != null){
                filetypeList.dispose();
            }
            filetypeList = new Menu(parent);
            
            Group[] groups = ColorerPlugin.getDefaultPF().getHRCParser().getGroups();

            FileType currentFileType = ((ColorerEditor)activeEditor).getFileType();
            
            /* Recursively creates menu of HRC FileTypes and Groups */
            for(int idx = 0; idx < groups.length; idx++){
                fillMenuTree(groups[idx], filetypeList, currentFileType);
            }
            return filetypeList;
        }

        public Menu getMenu(Menu parent) {
            return null;
        }

        public void dispose() {
            if (filetypeList != null)
                filetypeList.dispose();
        }
    
    }


    TextOperationAction fToggle;
    TextOperationAction fExpandAll;
    TextOperationAction fCollapseAll;

    WordWrapAction fWordWrapAction = new WordWrapAction();
    
    public ColorerActionContributor() {

        hrcupdateAction.setText(Messages.get("editor.hrcupdate"));
        hrcupdateAction.setToolTipText(Messages.get("editor.hrcupdate.tooltip"));
        hrcupdateAction.setImageDescriptor(ImageStore.EDITOR_UPDATEHRC);
        hrcupdateAction.setHoverImageDescriptor(ImageStore.EDITOR_UPDATEHRC_A);
        hrcupdateAction.setActionDefinitionId("net.sf.colorer.eclipse.editors.hrcupdate");

        filetypeAction = new FileTypeActionMenu(Messages.get("editor.filetype"));
        filetypeAction.setToolTipText(Messages.get("editor.filetype.tooltip"));
        filetypeAction.setImageDescriptor(ImageStore.EDITOR_FILETYPE);
        filetypeAction.setHoverImageDescriptor(ImageStore.EDITOR_FILETYPE_A);
        filetypeAction.setActionDefinitionId("net.sf.colorer.eclipse.editors.choosetype");

        cursorRegionAction.setActionDefinitionId("net.sf.colorer.eclipse.editors.selectregion");       

        fToggle = new TextOperationAction(Messages.getResourceBundle(), "Projection.Toggle.", null, ProjectionViewer.TOGGLE, true);
        fToggle.setChecked(true);
        fToggle.setActionDefinitionId(IFoldingCommandIds.FOLDING_TOGGLE);
        
        fExpandAll = new TextOperationAction(Messages.getResourceBundle(), "Projection.ExpandAll.", null, ProjectionViewer.EXPAND_ALL, true);
        fExpandAll.setActionDefinitionId(IFoldingCommandIds.FOLDING_EXPAND_ALL);
        
        fCollapseAll = new TextOperationAction(Messages.getResourceBundle(), "Projection.CollapseAll.", null, ProjectionViewer.COLLAPSE_ALL, true);
        fCollapseAll.setActionDefinitionId(IFoldingCommandIds.FOLDING_COLLAPSE_ALL);

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

    /**
     * 
     * @see org.eclipse.ui.editors.text.TextEditorActionContributor#setActiveEditor(org.eclipse.ui.IEditorPart)
     */
    public void setActiveEditor(IEditorPart editor) {
        super.setActiveEditor(editor);
        if (editor instanceof ColorerEditor){
            activeEditor = (ColorerEditor) editor;
        }else{
            activeEditor = null;
        }

        fToggle.setEditor(activeEditor);
        fExpandAll.setEditor(activeEditor);
        fCollapseAll.setEditor(activeEditor);
        fWordWrapAction.setEditor(activeEditor);

        activeEditor.setAction(hrcupdateAction.getActionDefinitionId(), hrcupdateAction);
        activeEditor.setAction(filetypeAction.getActionDefinitionId(), filetypeAction);
        activeEditor.setAction(pairMatchAction.getActionDefinitionId(), pairMatchAction);
        activeEditor.setAction(pairSelectAction.getActionDefinitionId(), pairSelectAction);
        activeEditor.setAction(pairSelectContentAction.getActionDefinitionId(), pairSelectContentAction);
        activeEditor.setAction(cursorRegionAction.getActionDefinitionId(), cursorRegionAction);
        activeEditor.setAction(fWordWrapAction.getActionDefinitionId(), fWordWrapAction);
        
        activeEditor.setAction(fToggle.getActionDefinitionId(), fToggle); //$NON-NLS-1$
        activeEditor.setAction(fExpandAll.getActionDefinitionId(), fExpandAll); //$NON-NLS-1$
        activeEditor.setAction(fCollapseAll.getActionDefinitionId(), fCollapseAll); //$NON-NLS-1$
        
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
 * Portions created by the Initial Developer are Copyright (C) 1999-2005
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