package net.sf.colorer.eclipse;

import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;
import java.util.Iterator;

import net.sf.colorer.editor.BaseEditor;
import net.sf.colorer.impl.BaseEditorNative;
import net.sf.colorer.impl.ReaderLineSource;
import net.sf.colorer.viewer.ParsedLineWriter;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;

class EscapedWriter extends Writer{
  Writer writer;
  EscapedWriter(Writer corewriter){
    writer = corewriter;
  }
  public void write(char[] cbuf, int off, int len) throws IOException {
    for(int idx = off; idx < off+len; idx++){
      if (cbuf[idx] == '<') writer.write("&lt;");
      else if (cbuf[idx] == '&') writer.write("&amp;");
      else writer.write(cbuf[idx]);
    }
  }
  public void close() throws IOException {}
  public void flush() throws IOException {}
};

public class HTMLGeneratorAction implements IObjectActionDelegate  {

  IWorkbenchPart workbenchPart;

  public void setActivePart(IAction action, IWorkbenchPart part) {
    workbenchPart = part;
  }

	public void selectionChanged(IAction arg0, ISelection arg1) {}

  public void run(IAction action) {
    ISelectionProvider selectionProvider = workbenchPart.getSite().getSelectionProvider();
    StructuredSelection selection = (StructuredSelection) selectionProvider.getSelection();
    
    StringBuffer fileNames = new StringBuffer();
    String lastFileName = "";
    String filePath = "./";
    int num = 0;
    try{
      Iterator iterator = selection.iterator();
      while (iterator.hasNext()) {
        Object obj = iterator.next();
        if (!(obj instanceof IResource)) {
          continue;
        }
        IResource resource = (IResource)obj;
        if (resource == null || resource.getType() != IResource.FILE) return;
  
        IFile iFile = (IFile) resource;
        String fileLocation = iFile.getLocation().toString();
        lastFileName = iFile.getName();
        File file = new File(fileLocation);
        fileNames.append(lastFileName).append("\n");
      
        if (num == 0){
          FileDialog dialog = new FileDialog (new Shell(), SWT.SAVE);
          dialog.setFilterNames (new String [] {"HTML Files", "All files"});
          dialog.setFilterExtensions (new String [] {"*.html", "*.*"});
          dialog.setFileName(lastFileName+".html");
          String fileName = dialog.open();
          if (fileName == null) return;
          filePath = new File(fileName).getParentFile().getAbsolutePath();
        }
              
        ReaderLineSource rls = new ReaderLineSource(new FileReader(file));
        BaseEditor be = new BaseEditorNative(EclipsecolorerPlugin.getDefault().getParserFactory(), rls);
        be.setRegionCompact(true);
        be.setRegionMapper("rgb", EclipsecolorerPlugin.getDefault().getPreferenceStore().getString(PreferencePage.HRD_SET));
        be.lineCountEvent(rls.getLineCount());
        be.visibleTextEvent(0, rls.getLineCount());
        be.chooseFileType(file.getName());

        Writer commonWriter = new FileWriter(filePath + "/"+lastFileName+".html");
        Writer escapedWriter = new EscapedWriter(commonWriter);

        commonWriter.write("<html>\n<head>\n<style></style>\n</head>\n<body><pre>\n");

        commonWriter.write("Created with Colorer-take5 Library. Type '"+be.getFileType()+"'\n\n");

        for(int i = 0; i < rls.getLineCount(); i++){
          ParsedLineWriter.htmlRGBWrite(commonWriter, escapedWriter, rls.getLine(i), be.getLineRegions(i));
          commonWriter.write("\n");
        };
        commonWriter.write("</pre></body></html>\n");
        commonWriter.close();
        num++;
      };
      MessageDialog.openInformation(null, Messages.getString("htmlgen.done"),
                Messages.format("htmlgen.done.msg",
                    new Object[]{ String.valueOf(num), filePath, fileNames.toString()}));
    }catch(Exception e){
      MessageDialog.openError(null, Messages.getString("htmlgen.fault"),
              Messages.format("htmlgen.fault.msg",
                       new Object[]{ String.valueOf(num), filePath, e, lastFileName}) );

    }
  }
  
}