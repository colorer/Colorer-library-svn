package net.sf.colorer.eclipse;

import java.util.Hashtable;
import java.util.Vector;

import net.sf.colorer.FileType;
import net.sf.colorer.Group;
import net.sf.colorer.ParserFactory;
import net.sf.colorer.impl.Logger;

import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.CheckboxCellEditor;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;

class TypeParametersContentProvider implements  IStructuredContentProvider{
    
    public FileType type;
    
    public Object[] getElements(Object inputElement) {
        type = (FileType)inputElement;
        return type.getParameters();
    }

    public void dispose() {
    }

    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
        type = (FileType)newInput;
    }
}

class TypeParametersLabelProvider extends LabelProvider implements ITableLabelProvider{
    Image grImage = ImageStore.EDITOR_CUR_GROUP.createImage();
    Image grImageDis = ImageStore.EDITOR_GROUP.createImage();
    
    TypeParametersContentProvider contentProvider;
    
    public TypeParametersLabelProvider(TypeParametersContentProvider cp){
        contentProvider = cp;
    }

    public Image getColumnImage(Object element, int columnIndex) {
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
        if (columnIndex == 0) {
            return type.getParameterDescription((String)element);
        }else {
            return type.getParameterValue((String)element);
        }
    }
    
}

class FileTypesContentProvider implements  ITreeContentProvider{
    
    Vector groups;
    Hashtable groupElements;

    public FileTypesContentProvider() {}
    
    public Object[] getChildren(Object parentElement) {
        if (parentElement instanceof Group){
            Group[] g = ((Group)parentElement).getGroups();
            FileType[] ft = ((Group)parentElement).getFileTypes();
            Object[] children = new Object[g.length + ft.length];
            System.arraycopy(ft, 0, children, 0, ft.length);
            System.arraycopy(g, 0, children, ft.length, g.length);
            return children;
        }
        return null;
    }
    public boolean hasChildren(Object element) {
        if (element instanceof Group){
            return ((Group)element).getGroups().length != 0 || ((Group)element).getFileTypes().length != 0;
        }
        return false;
    }
    public Object getParent(Object element) {
        if (element instanceof FileType){
            return ((FileType)element).getGroup();
        }
        return null;
    }
    public Object[] getElements(Object inputElement) {
        ParserFactory pf = (ParserFactory)inputElement;
        return pf.getHRCParser().getGroups();
    }
    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
    }
    public void dispose() {
    }
}

class FileTypesLabelProvider extends LabelProvider{

    Image grImage = ImageStore.EDITOR_CUR_GROUP.createImage();
    Image ftImage = ImageStore.EDITOR_FILETYPE.createImage();

    public Image getImage(Object element) {
      	if (element instanceof FileType){
            return ftImage;
      	}
        if (element instanceof Group) {
            return grImage;
        }
        return null;
    }
    public String getText(Object element) {
        if (element instanceof FileType){
            FileType type = (FileType)element;
            return type.getDescription();
        }
        if (element instanceof Group){
            return ((Group)element).getDescription();
        }
        return element.toString();
    }
}

public class FileTypePreferencePage extends PreferencePage implements IWorkbenchPreferencePage{

    private Composite root;
    private Tree typesTree;
    private Button spacesAsTabs;
    private Text spacesTabNumber;
    private Combo hrdSchemas;
    private Table typePropertiesTable;
    TypeParametersContentProvider typePropertiesProvider;
    TableViewer typePropertiesViewer;

    public FileTypePreferencePage(){
        //super(, FieldEditorPreferencePage.GRID);
        setPreferenceStore(ColorerPlugin.getDefault().getPreferenceStore());
    }
    
    public void init(IWorkbench iworkbench){}

    public boolean performOk() {
        return true;
    }
    public boolean performCancel() {
        return true;
    }

    
    
  public Control createContents(Composite parent){
    Composite composite = new Composite(parent, SWT.NONE);
    composite.setLayoutData(new GridData(GridData.FILL_VERTICAL));
    composite.setLayout(new FillLayout(SWT.VERTICAL));

    ParserFactory pf = ColorerPlugin.getDefault().getParserFactory();
    typePropertiesProvider = new TypeParametersContentProvider();
    
    {
      TreeViewer tv = new TreeViewer(composite, SWT.BORDER|SWT.V_SCROLL|SWT.FULL_SELECTION);

      tv.setContentProvider(new FileTypesContentProvider());
      tv.setLabelProvider(new FileTypesLabelProvider());
      tv.setInput(pf);
      tv.addPostSelectionChangedListener(new ISelectionChangedListener(){
        public void selectionChanged(SelectionChangedEvent event) {
            Object selection = ((IStructuredSelection)event.getSelection()).getFirstElement();
            if (selection != null && selection instanceof FileType) {
                typePropertiesProvider.type = (FileType)selection;
                typePropertiesViewer.setInput(selection);
            }else {
                typePropertiesProvider.type = null;
                typePropertiesViewer.setInput(null);
            }
            typePropertiesViewer.refresh();
        }
      });
    }
    {
      typePropertiesViewer = new TableViewer(composite, SWT.V_SCROLL|SWT.BORDER|SWT.FULL_SELECTION);
      typePropertiesTable = typePropertiesViewer.getTable();
      typePropertiesViewer.setContentProvider(typePropertiesProvider);
      typePropertiesViewer.setLabelProvider(new TypeParametersLabelProvider(typePropertiesProvider));

      TableColumn tc = new TableColumn(typePropertiesTable, SWT.LEFT);
      tc.setText("Parameter");
      tc.setWidth(300);
      tc = new TableColumn(typePropertiesTable, SWT.LEFT);
      tc.setText("Value");
      tc.setWidth(90);

      ICellModifier cellModifier = new ICellModifier() {
            public Object getValue(Object element, String property) {
                Logger.trace("Preference", "getValue:"+element.toString() + " - " + property);
                String pval = typePropertiesProvider.type.getParameterValue(element.toString());
                return new Boolean(pval.equals("true"));
            }
            public boolean canModify(Object element, String property) {
                Logger.trace("Preference", "canModify:"+element.toString() + " - " + property);
                String pval = typePropertiesProvider.type.getParameterDefaultValue(element.toString());
                return pval.equals("true") || pval.equals("false");
            }
            public void modify(Object element, String property, Object value) {
                Logger.trace("Preference", "modify:"+element.toString() + " - " + property + " - " + value);
                if (value instanceof Boolean) {
                    boolean i = ((Boolean)value).booleanValue();
                    String pname = (String)((TableItem)element).getData();
                    typePropertiesProvider.type.setParameterValue(pname, i ? "true" : "false");
                    typePropertiesViewer.refresh();
                }
            }
      };


      String []arr = new String[] { "true", "false"};
      CellEditor cellEditors[] = new CellEditor[typePropertiesViewer.getTable().getColumnCount()];
      //CellEditor paramCellEditor = new ComboBoxCellEditor(typePropertiesTable, arr, SWT.READ_ONLY | SWT.DROP_DOWN);
      CellEditor paramCellEditor = new CheckboxCellEditor(typePropertiesTable);
      
      cellEditors[1] = paramCellEditor;
      typePropertiesViewer.setCellEditors(cellEditors);
      typePropertiesViewer.setCellModifier(cellModifier);
      
      typePropertiesViewer.setColumnProperties(new String[] { "name", "value"});

      typePropertiesTable.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_FILL|GridData.VERTICAL_ALIGN_FILL));
      //, true, true, 2, 1
      typePropertiesTable.setHeaderVisible(true);
      typePropertiesTable.setLinesVisible(true);

      typePropertiesViewer.setInput(null);
    }
    return composite;
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
