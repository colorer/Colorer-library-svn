package net.sf.colorer.eclipse.view;

import net.sf.colorer.swt.dialog.ResourceManager;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.part.*;
import org.eclipse.swt.layout.*;
import org.eclipse.jface.action.*;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.*;

/**
 * This view manages current color styles in Colorer plugin,
 * allows to change some of them and save changes into separate
 * files.
 */
public class HRDEditorView extends ViewPart {
	
  private Composite composite;
  
	private Action action1;
	private Action action2;
	private Action doubleClickAction;

	/**
	 * The view constructor.
	 */
	public HRDEditorView() {
	}

	/**
	 * This is a callback that will allow us
	 * to create the viewer and initialize it.
	 */
	public void createPartControl(Composite parent) {

    composite = createComposite(parent);

		makeActions();
		//hookContextMenu();
		contributeToActionBars();
	}

  Composite createComposite(Composite parent){
    Composite composite = new Composite(parent, SWT.NONE);
    composite.setLayout(new GridLayout());
    {
      final Table table = new Table(composite, SWT.BORDER);
      table.setLayoutData(new GridData(GridData.FILL_BOTH));
      table.setLinesVisible(true);
      table.setHeaderVisible(true);
    }
    {
      final Composite composite_1 = new Composite(composite, SWT.BORDER);
      composite_1.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_FILL));
      final GridLayout gridLayout = new GridLayout();
      gridLayout.numColumns = 3;
      composite_1.setLayout(gridLayout);
      {
        final Label label = new Label(composite_1, SWT.BORDER | SWT.CENTER);
        final GridData gridData = new GridData(GridData.GRAB_HORIZONTAL | GridData.HORIZONTAL_ALIGN_CENTER | GridData.VERTICAL_ALIGN_FILL);
        gridData.verticalSpan = 2;
        label.setLayoutData(gridData);
        label.setBackground(ResourceManager.getColor(SWT.COLOR_WHITE));
        label.setText(" foreground color");
      }
      {
        final Button button = new Button(composite_1, SWT.NONE);
        final GridData gridData = new GridData(GridData.HORIZONTAL_ALIGN_FILL);
        gridData.verticalSpan = 2;
        button.setLayoutData(gridData);
        button.setText("store ->");
      }
      {
        final Label label = new Label(composite_1, SWT.BORDER | SWT.CENTER);
        label.setBackground(ResourceManager.getColor(SWT.COLOR_WHITE));
        final GridData gridData = new GridData(GridData.GRAB_HORIZONTAL | GridData.HORIZONTAL_ALIGN_CENTER | GridData.VERTICAL_ALIGN_FILL);
        gridData.verticalSpan = 2;
        label.setLayoutData(gridData);
        label.setText("stored color");
      }
      {
        final Label label = new Label(composite_1, SWT.BORDER);
        label.setBackground(ResourceManager.getColor(SWT.COLOR_WHITE));
        final GridData gridData = new GridData(GridData.GRAB_HORIZONTAL | GridData.HORIZONTAL_ALIGN_CENTER | GridData.VERTICAL_ALIGN_FILL);
        gridData.verticalSpan = 2;
        label.setLayoutData(gridData);
        label.setText(" background color");
      }
      {
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
        final GridData gridData = new GridData(GridData.GRAB_HORIZONTAL | GridData.HORIZONTAL_ALIGN_CENTER | GridData.VERTICAL_ALIGN_FILL);
        gridData.verticalSpan = 2;
        label.setLayoutData(gridData);
        label.setText("stored color");
      }
    }
    {
      final Button button = new Button(composite, SWT.NONE);
      button.setText("Save HRD As...");
    }
    //  DESIGNER: Add controls before this line.    
    return composite;
  }
  
  /*
	private void hookContextMenu() {
		MenuManager menuMgr = new MenuManager("#PopupMenu");
		menuMgr.setRemoveAllWhenShown(true);
		menuMgr.addMenuListener(new IMenuListener() {
			public void menuAboutToShow(IMenuManager manager) {
				HRDEditorView.this.fillContextMenu(manager);
			}
		});
		Menu menu = menuMgr.createContextMenu(composite);
    composite.setMenu(menu);
		//getSite().registerContextMenu(menuMgr, viewer);
	}
  */

	private void contributeToActionBars() {
		IActionBars bars = getViewSite().getActionBars();
		fillLocalPullDown(bars.getMenuManager());
		fillLocalToolBar(bars.getToolBarManager());
	}

	private void fillLocalPullDown(IMenuManager manager) {
		manager.add(action1);
		manager.add(new Separator());
		manager.add(action2);
	}
  
  /*
	private void fillContextMenu(IMenuManager manager) {
		manager.add(action1);
		manager.add(action2);
		// Other plug-ins can contribute there actions here
		manager.add(new Separator(IWorkbenchActionConstants.MB_ADDITIONS));
	}
  */
	
	private void fillLocalToolBar(IToolBarManager manager) {
		manager.add(action1);
		manager.add(action2);
	}

	private void makeActions() {
		action1 = new Action() {
			public void run() {
				showMessage("Action 1 executed");
			}
		};
		action1.setText("Action 1");
		action1.setToolTipText("Action 1 tooltip");
		action1.setImageDescriptor(PlatformUI.getWorkbench().getSharedImages().
			getImageDescriptor(ISharedImages.IMG_OBJS_INFO_TSK));
		
		action2 = new Action() {
			public void run() {
				showMessage("Action 2 executed");
			}
		};
		action2.setText("Action 2");
		action2.setToolTipText("Action 2 tooltip");
		action2.setImageDescriptor(PlatformUI.getWorkbench().getSharedImages().
				getImageDescriptor(ISharedImages.IMG_OBJS_INFO_TSK));
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
}