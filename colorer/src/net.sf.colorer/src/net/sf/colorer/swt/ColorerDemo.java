package net.sf.colorer.swt;

import java.util.*;

import net.sf.colorer.ParserFactory;

import org.eclipse.swt.*;
import org.eclipse.swt.custom.*;
import org.eclipse.swt.events.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.layout.*;
import org.eclipse.swt.widgets.*;

public class ColorerDemo {  
    Shell shell;
    ToolBar toolBar;
    StyledText text;
    TextColorer textColorer;

//    Images images = new Images();
    Vector cachedStyles = new Vector();
    Font font = null;
    
    boolean isBold = false;
    
    ExtendedModifyListener extendedModifyListener;
    VerifyKeyListener verifyKeyListener;
    LineStyleListener lineStyleListener;
    
    static ResourceBundle resources = ResourceBundle.getBundle("net.sf.colorer.swt.ColorerDemo");

Menu createEditMenu() {
    Menu bar = shell.getMenuBar ();
    Menu menu = new Menu (bar);
    
    MenuItem item = new MenuItem (menu, SWT.PUSH);
    item.setText (resources.getString("Cut_menuitem"));
    item.addSelectionListener(new SelectionAdapter() {
        public void widgetSelected(SelectionEvent event) {
            text.cut();
        }
    });

    item = new MenuItem (menu, SWT.PUSH);
    item.setText (resources.getString("Copy_menuitem"));
    item.addSelectionListener(new SelectionAdapter() {
        public void widgetSelected(SelectionEvent event) {
            text.copy();
        }
    });

    item = new MenuItem (menu, SWT.PUSH);
    item.setText (resources.getString("Paste_menuitem"));
    item.addSelectionListener(new SelectionAdapter() {
        public void widgetSelected(SelectionEvent event) {
            text.paste();
        }
    });

    new MenuItem (menu, SWT.SEPARATOR);
    
    item = new MenuItem (menu, SWT.PUSH);
    item.setText (resources.getString("Font_menuitem"));
    item.addSelectionListener(new SelectionAdapter() {
        public void widgetSelected(SelectionEvent event) {
            setFont();
        }
    });
    return menu;
}

void createStyledText() {
    text = new StyledText (shell, SWT.BORDER | SWT.MULTI | SWT.V_SCROLL | SWT.H_SCROLL);
    GridData spec = new GridData();
    spec.horizontalAlignment = GridData.FILL;
    spec.grabExcessHorizontalSpace = true;
    spec.verticalAlignment = GridData.FILL;
    spec.grabExcessVerticalSpace = true;
    text.setLayoutData(spec);
    
    text.setFont(new Font(shell.getDisplay(), "Courier New", 10, SWT.NORMAL));
    
    ParserFactory pf = new ParserFactory();
    textColorer = new TextColorer(pf, new ColorManager());
    textColorer.attach(text);
    textColorer.setFileType("xml");
    textColorer.setCross(true, true);
    textColorer.setRegionMapper("default", true);
}


void createMenuBar () {
    Menu bar = new Menu (shell, SWT.BAR);
    shell.setMenuBar (bar);

    Menu fileMenu = new Menu(bar);
    MenuItem mItem = new MenuItem(fileMenu, SWT.PUSH);
    mItem.setText(resources.getString("File_Exit_menuitem"));
    mItem.addSelectionListener(new SelectionAdapter() {
        public void widgetSelected(SelectionEvent event) {
            System.exit(0);
        }
    });
    MenuItem fileItem = new MenuItem (bar, SWT.CASCADE);
    fileItem.setText(resources.getString("File_menuitem"));
    fileItem.setMenu(fileMenu);

    MenuItem editItem = new MenuItem(bar, SWT.CASCADE);
    editItem.setText(resources.getString("Edit_menuitem"));
    editItem.setMenu(createEditMenu());
}

void createToolBar() {
/*  toolBar = new ToolBar(shell, SWT.NULL);
    
    ToolItem item = new ToolItem(toolBar, SWT.CHECK);
    item.setImage(images.Bold);
    item.addSelectionListener(new SelectionAdapter() {
        public void widgetSelected(SelectionEvent event) {
            bold(((ToolItem)event.widget).getSelection());
        }
    });
    
    item = new ToolItem(toolBar, SWT.SEPARATOR);

    item = new ToolItem(toolBar, SWT.PUSH);
    item.setImage(images.Red);
    item.addSelectionListener(new SelectionAdapter() {
        public void widgetSelected(SelectionEvent event) {
            fgColor(SWT.COLOR_RED);
        }
    });
    item = new ToolItem(toolBar, SWT.PUSH);
    item.setImage(images.Green);
    item.addSelectionListener(new SelectionAdapter() {
        public void widgetSelected(SelectionEvent event) {
            fgColor(SWT.COLOR_GREEN);
        }
    });
    item = new ToolItem(toolBar, SWT.PUSH);
    item.setImage(images.Blue);
    item.addSelectionListener(new SelectionAdapter() {
        public void widgetSelected(SelectionEvent event) {
            fgColor(SWT.COLOR_BLUE);
        }
    });
    
    item = new ToolItem(toolBar, SWT.SEPARATOR);

    item = new ToolItem(toolBar, SWT.PUSH);
    item.setImage(images.Erase);
    item.addSelectionListener(new SelectionAdapter() {
        public void widgetSelected(SelectionEvent event) {
            clear();
        }
    });*/
}
/*void displayError(String msg) {
    MessageBox box = new MessageBox(shell, SWT.ICON_ERROR);
    box.setMessage(msg);
    box.open();
}*/

void setFont() {
	    FontDialog fontDialog = new FontDialog(shell);
	    fontDialog.setFontData((text.getFont()).getFontData()[0]);
	    FontData fontData = fontDialog.open();
	    if(fontData != null) {
        if(font != null)
            font.dispose();
        font = new Font(shell.getDisplay(), fontData);
        text.setFont(font);
    }
}

void createShell (Display display) {
    shell = new Shell (display);
    shell.setText (resources.getString("window_title"));    
    GridLayout layout = new GridLayout();
    layout.numColumns = 1;
    shell.setLayout(layout);
    shell.addDisposeListener (new DisposeListener () {
        public void widgetDisposed (DisposeEvent e) {
            if (font != null) font.dispose();
        }
    });
}

public Shell open (Display display) {
    createShell(display);
    createMenuBar();
    createToolBar();
    createStyledText();
    shell.setSize(700, 500);
    shell.open();
    return shell;
}

public static void main (String [] args) {
    Display display = new Display ();
    ColorerDemo clrDemo = new ColorerDemo();
    Shell shell = clrDemo.open(display);
    while (!shell.isDisposed ())
        if (!display.readAndDispatch ()) display.sleep ();
    display.dispose ();
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
 * Cail Lomecb <ruiv@uic.nnov.ru>.
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