package net.sf.colorer.eclipse.editors;

import net.sf.colorer.FileType;
import net.sf.colorer.eclipse.ColorerPlugin;
import net.sf.colorer.eclipse.IColorerReloadListener;
import net.sf.colorer.eclipse.Messages;
import net.sf.colorer.eclipse.PreferencePage;
import net.sf.colorer.eclipse.ftpp.FileTypePreferencePage;
import net.sf.colorer.eclipse.jface.ColorerAnnotation;
import net.sf.colorer.eclipse.jface.ColorerAnnotationProvider;
import net.sf.colorer.eclipse.jface.ColorerFoldingProvider;
import net.sf.colorer.eclipse.jface.LineNumberRulerColumn_Fixed;
import net.sf.colorer.eclipse.jface.TextColorer;
import net.sf.colorer.eclipse.outline.ColorerContentOutlinePage;
import net.sf.colorer.eclipse.outline.OutlineSchemeElement;
import net.sf.colorer.editor.BaseEditor;
import net.sf.colorer.editor.OutlineItem;
import net.sf.colorer.handlers.LineRegion;
import net.sf.colorer.impl.CachedBaseEditor;
import net.sf.colorer.impl.Logger;
import net.sf.colorer.swt.ColorManager;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IInformationControl;
import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.jface.text.ITextViewerExtension2;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.jface.text.source.IVerticalRuler;
import org.eclipse.jface.text.source.IVerticalRulerColumn;
import org.eclipse.jface.text.source.projection.ProjectionSupport;
import org.eclipse.jface.text.source.projection.ProjectionViewer;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.events.VerifyEvent;
import org.eclipse.swt.events.VerifyListener;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IStorageEditorInput;
import org.eclipse.ui.editors.text.IFoldingCommandIds;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.texteditor.AbstractDecoratedTextEditorPreferenceConstants;
import org.eclipse.ui.texteditor.ITextEditorActionConstants;
import org.eclipse.ui.views.contentoutline.IContentOutlinePage;

public class ColorerEditor extends TextEditor implements IPropertyChangeListener{

    IPreferenceStore prefStore;
    ColorerSourceViewerConfiguration fColorerSVC;

    StyledText text;
    IDocument fDocument;
    TextColorer fTextColorer;
    BaseEditor fBaseEditor;

    ColorerContentOutlinePage contentOutliner;
    ProjectionSupport fProjectionSupport;
    ColorerFoldingProvider fFoldingProvider = new ColorerFoldingProvider();
    ColorerAnnotationProvider fAnnotationProvider = new ColorerAnnotationProvider();

    VerifyListener tabReplacer = new VerifyListener() {
        public void verifyText(VerifyEvent e) {
            if (e.text.equals("\t")) {
                StringBuffer tab = new StringBuffer();
                for (int idx = 0; idx < text.getTabs(); idx++)
                    tab.append(' ');
                e.text = tab.toString();
            }
        }
    };

    final class OutlineSelectionListener implements ISelectionChangedListener {
        
        public void selectionChanged(SelectionChangedEvent event) {
            if (fDocument == null || event.getSelection().isEmpty()){
                return;
            }
            try {
                OutlineItem el = (OutlineItem)((IStructuredSelection)event.getSelection()).getFirstElement();
                if (el instanceof OutlineSchemeElement) {
                    OutlineSchemeElement sel = (OutlineSchemeElement)el;
                    int selstart = fDocument.getLineOffset(sel.lno)+sel.pos;
                    int selength = fDocument.getLineOffset(sel.l2no) + sel.pos2 - selstart;
                    if (sel.l2no == 0 && sel.pos2 == 0 || selength <= 0){
                        selength = 0;
                    }
                    selectAndReveal(selstart, selength);
                }else{
                    selectAndReveal(fDocument.getLineOffset(el.lno)+el.pos, el.length);
                }
            }catch(BadLocationException e){
                Logger.error("ColorerEditor", "doubleClick", e);
            };
        }
    }    
    
    IColorerReloadListener fReloadListener = new IColorerReloadListener(){
        public void notifyReload() {
            if (Logger.TRACE){
                Logger.trace("ColorerEditor", "notifyReload()");
            }
            detachBaseEditor();
            fBaseEditor.dispose();
            fBaseEditor = null;
            relinkColorer();
        }
    };

    public ColorerEditor() {
      super();

      ColorerPlugin.getDefault().addReloadListener(fReloadListener);
      
      prefStore = ColorerPlugin.getDefault().getCombinedPreferenceStore();
      prefStore.addPropertyChangeListener(this);
      
      JFaceResources.getFontRegistry().addListener(this);

      fTextColorer =  new TextColorer(this);
      fColorerSVC = new ColorerSourceViewerConfiguration(this, fTextColorer);
      
      setSourceViewerConfiguration(fColorerSVC);
    }

    protected ISourceViewer createSourceViewer(Composite parent, IVerticalRuler ruler, int styles) {
        
        ISourceViewer viewer = new ProjectionViewer(parent, ruler,
                getOverviewRuler(), isOverviewRulerVisible(), styles);

        getSourceViewerDecorationSupport(viewer);
        
        return viewer;
    }
    
    public void createPartControl(Composite parent)
    {
        super.createPartControl(parent);

        text = getSourceViewer().getTextWidget();
        fDocument = getSourceViewer().getDocument();

        ProjectionViewer viewer =(ProjectionViewer)getSourceViewer();
        fProjectionSupport = new ProjectionSupport(viewer,getAnnotationAccess(), getSharedColors());
        fProjectionSupport.setHoverControlCreator(new IInformationControlCreator() {
            public IInformationControl createInformationControl(Shell shell) {
                return new ColorerSourceViewerInformationControl(shell);
              }
           });
        
        fProjectionSupport.addSummarizableAnnotationType(ColorerAnnotation.ERROR);
        fProjectionSupport.addSummarizableAnnotationType(ColorerAnnotation.WARNING);
        fProjectionSupport.addSummarizableAnnotationType(ColorerAnnotation.INFO);
        fProjectionSupport.addSummarizableAnnotationType(ColorerAnnotation.TASK);

        fProjectionSupport.install();
        viewer.doOperation(ProjectionViewer.TOGGLE);

//        setRulerContextMenuId("net.sf.colorer.eclipse.editor.ColorerEditor.RulerContext");
        
        relinkColorer();
    }
    
    protected String[] collectContextMenuPreferencePages() {
        String[] preferencePages = super.collectContextMenuPreferencePages();
        String[] extendedPreferencePages = new String[preferencePages.length+2];
        extendedPreferencePages[0] = PreferencePage.class.getName();
        extendedPreferencePages[1] = FileTypePreferencePage.class.getName();
        System.arraycopy(preferencePages, 0, extendedPreferencePages, 2, preferencePages.length);
        return extendedPreferencePages;
    };

    protected void initializeKeyBindingScopes(){
        setKeyBindingScopes(new String[] {
            "net.sf.colorer.eclipse.ColorerScope"
        });
    }
    
    /**
     * Forwards invalidate syntax request down to parser components
     */
    public void invalidateSyntax() {
        fTextColorer.invalidateSyntax();
    }
    
    /**
     * Selects filetype according to file name and first line of content
     */
    public void chooseFileType() {
        fTextColorer.chooseFileType(getTitle());
    }

    /**
     * Selects file's type according to the passed type name
     */
    public void setFileType(FileType type) {
        fTextColorer.setFileType(type);
    }
    
    /** Returns currently used file type */
    public FileType getFileType() {
        return fBaseEditor.getFileType();
    }

    /**
     * Reloads coloring highlighting in this editor
     */
    public void relinkColorer() {
        fBaseEditor = (BaseEditor)getAdapter(BaseEditor.class);

        // Install folding provider
        fFoldingProvider.install(this);
        fAnnotationProvider.install(this);

        fTextColorer.relink();
        fTextColorer.chooseFileType(getTitle());

        if (contentOutliner != null) {
            contentOutliner.attach(this);
        }

        propertyChange(null);
    }

    /** Tries to match paired construction */
    public void matchPair() {
        if (!fTextColorer.matchPair()) {
            showPairError();
        }
    }

    /** Selects paired construction */
    public void selectPair() {
        if (!fTextColorer.selectPair()) {
            showPairError();
        }
    }

    /** Selects content of paired construction */
    public void selectContentPair() {
        if (!fTextColorer.selectContentPair()) {
            showPairError();
        }
    }

    /**
     * Retrieves current LineRegion under caret.
     * 
     * @return LineRegion currently under Caret
     */
    public LineRegion getCaretRegion() {
        return fTextColorer.getCaretRegion();

    }

    void showPairError() {
        MessageDialog.openInformation(null, Messages.get("editor.pairerr.title"),
                Messages.get("editor.pairerr.msg"));
    }
    
    /**
     * Selects region under the cursor
     */
    public void selectCursorRegion() {
        LineRegion lr = getCaretRegion();
        if (lr == null) return;
        
        int loffset = text.getOffsetAtLine(text.getLineAtOffset(text.getCaretOffset()));
        int selstart = loffset+lr.start;
        int selend = lr.end-lr.start;
        if (lr.end == -1){
            selend = 0;
        }
        text.setSelectionRange(selstart, selend);
    }
    
    //----------------------------------------
    /*
     * Eclipse Bugs patching with line numbers ruler
     */
    protected IVerticalRulerColumn createLineNumberRulerColumn() {
        if (prefStore.getBoolean(PreferencePage.WORD_WRAP_PATCH)) {
            //super.createLineNumberRulerColumn();
            return new LineNumberRulerColumn_Fixed();
        }else{
            return super.createLineNumberRulerColumn();
        }
    }

    protected void editorContextMenuAboutToShow(IMenuManager parentMenu) {
        super.editorContextMenuAboutToShow(parentMenu);
        if (!fTextColorer.pairAvailable())
            return;
//        ColorerActionContributor ec = (ColorerActionContributor) getEditorSite().getActionBarContributor();

        parentMenu.insertBefore(ITextEditorActionConstants.GROUP_UNDO, new Separator("colorer"));
        addAction(parentMenu, "colorer", ColorerActionContributor.ACTION_ID_PAIRMATCH);
        addAction(parentMenu, "colorer", ColorerActionContributor.ACTION_ID_PAIRSELECT);
        addAction(parentMenu, "colorer", ColorerActionContributor.ACTION_ID_PAIRSELECTCONTENT);
    }
    
    protected void rulerContextMenuAboutToShow(IMenuManager menu) {
        super.rulerContextMenuAboutToShow(menu);

        MenuManager foldingsub = new MenuManager(Messages.get("Projection.Folding.label"));
        menu.insertBefore("settings", foldingsub);
        addAction(foldingsub, IFoldingCommandIds.FOLDING_TOGGLE);
        addAction(foldingsub, IFoldingCommandIds.FOLDING_EXPAND_ALL);
        addAction(foldingsub, IFoldingCommandIds.FOLDING_COLLAPSE_ALL);

        menu.insertBefore("settings", new Separator("ww"));
        addAction(menu, "ww", WordWrapAction.class.getName());
    }

    public void propertyChange(PropertyChangeEvent e) {
        
        if (e == null ||
            e.getProperty().equals(PreferencePage.USE_BACK) ||
            e.getProperty().equals(PreferencePage.HRD_SET) ||
            e.getProperty().startsWith(ColorerPlugin.HRD_SIGNATURE))
        {
            String hrd = ColorerPlugin.getDefault().getPropertyHRD(getFileType());
            if (hrd == null) {
                hrd = prefStore.getString(PreferencePage.HRD_SET);
            }
            fTextColorer.setRegionMapper(hrd, prefStore.getBoolean(PreferencePage.USE_BACK));
        }
        
        if (e == null ||
            e.getProperty().equals(PreferencePage.FULL_BACK))
        {
            fTextColorer.setFullBackground(prefStore.getBoolean(PreferencePage.FULL_BACK));
        }
        
        if (e == null ||
            e.getProperty().equals(PreferencePage.HORZ_CROSS) ||
            e.getProperty().equals(PreferencePage.VERT_CROSS))
        {
            fTextColorer.setCross(
                    prefStore.getBoolean(PreferencePage.HORZ_CROSS),
                    prefStore.getBoolean(PreferencePage.VERT_CROSS));
        }

        if (e == null ||
            e.getProperty().equals(PreferencePage.WORD_WRAP) ||
            e.getProperty().startsWith(ColorerPlugin.WORD_WRAP_SIGNATURE))
        {
            int ww = ColorerPlugin.getDefault().getPropertyWordWrap(getFileType());
            if (ww == -1) {
                ww = prefStore.getBoolean(PreferencePage.WORD_WRAP) ? 1 : 0;
            }
            text.setWordWrap(ww == 1);
        }
        if (e == null ||
            e.getProperty().equals(AbstractDecoratedTextEditorPreferenceConstants.EDITOR_TAB_WIDTH))
            if (text.getTabs() != prefStore.getInt(AbstractDecoratedTextEditorPreferenceConstants.EDITOR_TAB_WIDTH))
            {
                text.setTabs(prefStore.getInt(AbstractDecoratedTextEditorPreferenceConstants.EDITOR_TAB_WIDTH));
            }

        if (e == null ||
            e.getProperty().equals(PreferencePage.SPACES_FOR_TABS))
        {
            if (prefStore.getBoolean(PreferencePage.SPACES_FOR_TABS))
                text.addVerifyListener(tabReplacer);
            else
                text.removeVerifyListener(tabReplacer);
        }

        if (e == null ||
            e.getProperty().equals(PreferencePage.PAIRS_MATCH))
        {
            String pairs = prefStore.getString(PreferencePage.PAIRS_MATCH);
            int pmode = TextColorer.HLS_XOR;
            if (pairs.equals("PAIRS_OUTLINE"))
                pmode = TextColorer.HLS_OUTLINE;
            if (pairs.equals("PAIRS_OUTLINE2"))
                pmode = TextColorer.HLS_OUTLINE2;
            if (pairs.equals("PAIRS_NO"))
                pmode = TextColorer.HLS_NONE;

            if (getSourceViewer() instanceof ITextViewerExtension2)
            {
                fTextColorer.setPairsPainter(pmode);
            }
        }

        if (e == null ||
            e.getProperty().equals(PreferencePage.TEXT_FONT))
        {
            Font textFont = JFaceResources.getFont(PreferencePage.TEXT_FONT);
            text.setFont(textFont);
        }
    }

    public Object getAdapter(Class key)
    {
        if (key.equals(ISourceViewer.class)) {
            Assert.isNotNull(getSourceViewer());
            return getSourceViewer();
        }
        
        if (key.equals(ColorManager.class)) {
            return ColorerPlugin.getDefault().getColorManager();
        }

        if (key.equals(BaseEditor.class)) {
            if (fBaseEditor == null){
                fBaseEditor = new CachedBaseEditor(ColorerPlugin.getDefaultPF(),
                        new DocumentLineSource(getSourceViewer().getDocument()));
                fBaseEditor.setRegionCompact(true);                
            }
            return fBaseEditor;
        }
        
        if (key.equals(TextColorer.class)) {
            return fTextColorer;
        }

        if (key.equals(IContentOutlinePage.class)) {
            IEditorInput input = getEditorInput();
            if (input instanceof IStorageEditorInput) {
                if (contentOutliner == null){
                    contentOutliner = new ColorerContentOutlinePage();
                    contentOutliner.addSelectionChangedListener(new OutlineSelectionListener());
                    contentOutliner.attach(this);
                }
                return contentOutliner;
            }
        }
        
        if (fProjectionSupport != null) { 
            Object adapter= fProjectionSupport.getAdapter(getSourceViewer(), key); 
            if (adapter != null) {
                return adapter;
            }
        }

//        if (key == IShowInTargetList.class) {
//            return new IShowInTargetList() {
//                public String[] getShowInTargetIds() {
//                    return new String[] { JavaUI.ID_PACKAGES, IPageLayout.ID_RES_NAV };
//                }
//            };
//        }
        
        return super.getAdapter(key);
    }
    
    void detachBaseEditor() {
        if (contentOutliner != null) {
            contentOutliner.detach();
        }
        fFoldingProvider.uninstall();
        fAnnotationProvider.uninstall();
    }

    public void dispose() {
        super.dispose();
        
        detachBaseEditor();

        prefStore.removePropertyChangeListener(this);
        JFaceResources.getFontRegistry().removeListener(this);
        ColorerPlugin.getDefault().removeReloadListener(fReloadListener);
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
 * The Original Code is the Colorer Library
 *
 * The Initial Developer of the Original Code is
 * Igor Russkih <irusskih at gmail dot com>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2007
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