package net.sf.colorer.eclipse.view;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import net.sf.colorer.FileType;
import net.sf.colorer.HRCParser;
import net.sf.colorer.ParserFactory;
import net.sf.colorer.Region;
import net.sf.colorer.eclipse.ColorerPlugin;
import net.sf.colorer.eclipse.PreferencePage;
import net.sf.colorer.handlers.RegionMapper;
import net.sf.colorer.handlers.StyledRegion;
import net.sf.colorer.impl.Logger;
import net.sf.colorer.swt.dialog.ResourceManager;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IPropertyListener;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;

/**
 * This view represents all HRC regions in a tree-like form,
 * allowing to navigate them and discover their relation.
 */
public class HRCRegionsTreeView extends ViewPart implements IPropertyChangeListener {

    private Composite composite;
    private TreeViewer treeViewer;
    
    private HRCParser hrcParser;

    private Action refreshAction, loadAllAction, linkToEditorAction;
    
    private IPreferenceStore prefStore;
    
    RegionMapper regionMapper;
    Label fg_label, bg_label;
    Color foreColor, backColor;

    /**
     * The view constructor.
     */
    public HRCRegionsTreeView() {
    }

    /**
     * This is a callback that will allow us
     * to create the viewer and initialize it.
     */
    public void createPartControl(Composite parent) {
        composite = createComposite(parent);
        makeActions();
        contributeToActionBars();
        
        prefStore = ColorerPlugin.getDefault().getPreferenceStore();
        prefStore.addPropertyChangeListener(this);
        propertyChange(null);
    }
    
    public void propertyChange(PropertyChangeEvent event) {
        
        if (regionMapper != null){
            regionMapper.dispose();
        }
        try{
            regionMapper = ColorerPlugin.getDefault().getParserFactory().
                createStyledMapper(StyledRegion.HRD_RGB_CLASS,
                        prefStore.getString(PreferencePage.HRD_SET));
        }catch(Exception e){
            if (Logger.ERROR){
                Logger.error("HRCRegionsTreeView", "createStyledMapper:", e);
            }
        }
    }    

    Composite createComposite(Composite parent){
        Composite composite = new Composite(parent, SWT.NONE);
        composite.setLayout(new GridLayout(1, true));
        {
            final Composite composite_1 = new Composite(composite, SWT.NONE);
            composite_1.setLayout(new FillLayout());
            composite_1.setLayoutData(new GridData(GridData.FILL_BOTH));

            treeViewer = new TreeViewer(composite_1, SWT.SINGLE | SWT.H_SCROLL | SWT.V_SCROLL | SWT.BORDER);
            treeViewer.setContentProvider(new RegionContentProvider());
            treeViewer.setLabelProvider(new RegionTreeLabelProvider());
            
            hrcParser = ColorerPlugin.getDefault().getParserFactory().getHRCParser();
            treeViewer.setInput(hrcParser);

            treeViewer.addDoubleClickListener(new IDoubleClickListener(){
                public void doubleClick(DoubleClickEvent event) {
                    Region sel = getSelectedRegion(event.getSelection());
                    if (sel == null){
                        return;
                    }
                    Logger.trace("doubleClick", sel);
                    if (treeViewer.getExpandedState(sel)){
                        treeViewer.collapseToLevel(sel, 1);
                    }else{
                        treeViewer.expandToLevel(sel, 1);
                    }
                    
                }
            });

            treeViewer.addPostSelectionChangedListener(new ISelectionChangedListener(){
                public void selectionChanged(SelectionChangedEvent event) {
                    Region sel = getSelectedRegion(event.getSelection());
                    if (sel == null){
                        return;
                    }

                    StyledRegion sr = (StyledRegion)regionMapper.getRegionDefine(sel);
                    
                    foreColor = ResourceManager.getColor(SWT.COLOR_BLACK);
                    backColor = ResourceManager.getColor(SWT.COLOR_WHITE);
                    
                    if (sr != null){
                        if (sr.bfore){
                            foreColor.dispose();
                            foreColor = new Color(Display.getCurrent(), sr.fore>>16, sr.fore>>8 & 0xFF, sr.fore & 0xFF);
                        }
                        if (sr.bback){
                            backColor.dispose();
                            backColor = new Color(Display.getCurrent(), sr.back>>16, sr.back>>8 & 0xFF, sr.back & 0xFF);
                        }
                    }
                    
                    fg_label.setBackground(foreColor);
                    //fg_label.setForeground(ResourceManager.getColor(SWT.COLOR_RED));

                    bg_label.setForeground(foreColor);
                    bg_label.setBackground(backColor);
                    
                }
            });
            
        }
        
        {
            final Composite composite_1 = new Composite(composite, SWT.BORDER);
            composite_1.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
            final GridLayout gridLayout = new GridLayout();
            gridLayout.numColumns = 3;
            composite_1.setLayout(gridLayout);
            {
                fg_label = new Label(composite_1, SWT.BORDER | SWT.CENTER | SWT.VERTICAL);
                final GridData gridData = new GridData(GridData.GRAB_HORIZONTAL
                        | GridData.HORIZONTAL_ALIGN_CENTER | GridData.VERTICAL_ALIGN_FILL);
                gridData.verticalSpan = 2;
                gridData.heightHint = 30;
                fg_label.setLayoutData(gridData);
                fg_label.setBackground(ResourceManager.getColor(SWT.COLOR_WHITE));
                fg_label.setText("Foreground");
            }
            {
                bg_label = new Label(composite_1, SWT.BORDER | SWT.CENTER | SWT.VERTICAL);
                bg_label.setBackground(ResourceManager.getColor(SWT.COLOR_WHITE));
                final GridData gridData = new GridData(GridData.GRAB_HORIZONTAL
                        | GridData.HORIZONTAL_ALIGN_CENTER | GridData.VERTICAL_ALIGN_FILL);
                gridData.verticalSpan = 2;
                gridData.heightHint = 30;
                bg_label.setLayoutData(gridData);
                bg_label.setText("Background");
            }
/*            {
                final Button button = new Button(composite_1, SWT.NONE);
                button.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_FILL));
                final GridData gridData = new GridData(GridData.HORIZONTAL_ALIGN_FILL);
                gridData.verticalSpan = 2;
                button.setLayoutData(gridData);
                button.setText("<- restore");
            }
            {
                final Label label = new Label(composite_1, SWT.BORDER);
                label.setBackground(ResourceManager.getColor(SWT.COLOR_WHITE));
                final GridData gridData = new GridData(GridData.GRAB_HORIZONTAL
                        | GridData.HORIZONTAL_ALIGN_CENTER | GridData.VERTICAL_ALIGN_FILL);
                gridData.verticalSpan = 2;
                label.setLayoutData(gridData);
                label.setText("stored color");
            }
*/            
        }
/*
        {
            final Button button = new Button(composite, SWT.NONE);
            button.setText("Save HRD As...");
            button.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_END));
        }
*/        

        return composite;
    }
    
    Region getSelectedRegion(ISelection selection){
        if (selection instanceof StructuredSelection){
            StructuredSelection sel = (StructuredSelection)selection;
            if (sel.isEmpty()){
                return null;
            }
            Region region = (Region)sel.getFirstElement();
            return region;
        }
        return null;
    }

    private void contributeToActionBars() {
        IActionBars bars = getViewSite().getActionBars();
        //fillLocalPullDown(bars.getMenuManager());
        fillLocalToolBar(bars.getToolBarManager());
    }

    private void fillLocalPullDown(IMenuManager manager) {
        manager.add(refreshAction);
        manager.add(new Separator());
    }

    private void fillLocalToolBar(IToolBarManager manager) {
        manager.add(refreshAction);
        manager.add(loadAllAction);
        manager.add(linkToEditorAction);
    }

    private void makeActions() {

        refreshAction = new Action("Refresh",
                PlatformUI.getWorkbench().getSharedImages().
                getImageDescriptor(ISharedImages.IMG_TOOL_COPY))
        {
            public void run() {
                hrcParser = ColorerPlugin.getDefault().getParserFactory().getHRCParser();
                treeViewer.setInput(hrcParser);
            }
        };
        refreshAction.setToolTipText("Refresh tree according to current HRC state");
        
        loadAllAction = new Action("Load All",
                PlatformUI.getWorkbench().getSharedImages().
                getImageDescriptor(ISharedImages.IMG_OBJ_ELEMENT))
        {
            public void run() {
                BusyIndicator.showWhile(treeViewer.getTree().getDisplay(), new Runnable(){
                    public void run() {
                        for(Enumeration e = hrcParser.enumerateFileTypes(); e.hasMoreElements();){
                            FileType ft = (FileType)e.nextElement();
                            ft.getBaseScheme();
                        }
                    }
                });
            }
        };
        loadAllAction.setToolTipText("Loads all available HRC resources to see Full Region Tree");
        
        linkToEditorAction = new Action("Link", Action.AS_CHECK_BOX){
            public void run() {
                //
            }
        };
        linkToEditorAction.setImageDescriptor(PlatformUI.getWorkbench().getSharedImages().getImageDescriptor(ISharedImages.IMG_TOOL_COPY));
        linkToEditorAction.setToolTipText("Links selection with current editor region under cursor");
        
    }

    private void showMessage(String message) {
         MessageDialog.openInformation(
            composite.getShell(),
            "Sample View",
            message);
    }

    /**
     * Passing the focus request to the viewer's control.
     */
    public void setFocus() {
        composite.setFocus();
    }
    
    
    public void dispose() {
        if (foreColor != null){
            foreColor.dispose();
        }
        if (backColor != null){
            backColor.dispose();
        }
        regionMapper.dispose();
        super.dispose();
    }
    
}


class RegionContentProvider implements ITreeContentProvider {

    HRCParser hp;
    
    Hashtable children = new Hashtable();
    
    public void dispose() {
        children = null;
    }

    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
        hp = (HRCParser)newInput;
        children.clear();
    }

    public Object[] getElements(Object inputElement) {
        children.clear();
        HRCParser hp = (HRCParser)inputElement;
        if (inputElement instanceof HRCParser){
            Vector list = new Vector();
            int idx = 1;
            while(true){
                Region region = hp.getRegion(idx);
                Logger.trace("RegionsTree", "Region: "+region);
                if (region == null){
                    break;
                }
                if (region.getParent() == null){
                    list.add(region);
                }else{
                    addChild(region.getParent(), region);
                }
                idx++;
            }
            return list.toArray();
        }else{
            return null;
        }
    }
    
    void addChild(Region parent, Region child){
        Vector list = (Vector)children.get(parent);
        if (list == null){
            list = new Vector();
            children.put(parent, list);
        }
        list.add(child);
    }

    public Object[] getChildren(Object parentElement) {
        Vector list = (Vector)children.get(parentElement);
        if (list == null){
            return null;
        }else{
            return list.toArray();
        }
    }

    public Object getParent(Object element) {
        return null;
    }

    public boolean hasChildren(Object element) {
        Vector list = (Vector)children.get(element);
        return list != null;
    }
}


class RegionTreeLabelProvider extends LabelProvider {
    
    public String getText(Object element) {
        if (element instanceof Region){
            Region reg = (Region)element;
            if (reg.getDescription() == null){
                return reg.getName();
            }else{
                return reg.getName() + " - " + reg.getDescription();
            }
        }else{
            return super.getText(element);
        }
    }
    
}
