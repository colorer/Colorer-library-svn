package net.sf.colorer.eclipse;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.Vector;

import net.sf.colorer.ParserFactory;
import net.sf.colorer.dialogs.ActionListener;
import net.sf.colorer.dialogs.GeneratorDialog;
import net.sf.colorer.impl.ReaderLineSource;
import net.sf.colorer.viewer.HTMLGenerator;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.StructuredSelection;
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

    GeneratorDialog gd = new GeneratorDialog();

    Vector fileList = new Vector();
    Iterator iterator = selection.iterator();
    while (iterator.hasNext()) {
      Object obj = iterator.next();
      
      String fileLocation = null;
      
      if (obj instanceof IResource) {
        IResource resource = (IResource)obj;
        if (resource == null || resource.getType() != IResource.FILE){
          continue;
        }
        IFile iFile = (IFile) resource;
        fileLocation = iFile.getLocation().toString(); 
      }
      /*
      else if (obj instanceof org.eclipse.jdt.core.IJavaElement){
        System.out.println(((org.eclipse.jdt.core.IJavaElement)obj).getElementName());        
      }
      */
      
      if (fileLocation == null){
        continue;
      }
      fileList.addElement(fileLocation);
    }
    gd.setFileList(fileList);

    final ParserFactory pf = EclipsecolorerPlugin.getDefault().getParserFactory();
    Vector hrdSchemas = new Vector();
    for(Enumeration hrds = pf.enumerateHRDInstances("rgb"); hrds.hasMoreElements();){
      final String hrd_name = (String)hrds.nextElement();
      final String hrd_descr = pf.getHRDescription("rgb", hrd_name);
      hrdSchemas.addElement(hrd_descr);
      hrdSchemas.addElement(hrd_name);
    }

    gd.setHRDSchema(hrdSchemas);

    IPreferenceStore ps = EclipsecolorerPlugin.getDefault().getPreferenceStore();
    gd.setPrefix(ps.getString("g.Prefix"));
    gd.setSuffix(ps.getString("g.Suffix"));
    gd.setHRDSchema(ps.getString("g.HRDSchema"));
    gd.setHtmlHeaderFooter(ps.getBoolean("g.HtmlHeaderFooter"));
    gd.setInfoHeader(ps.getBoolean("g.InfoHeader"));
    gd.setUseLineNumbers(ps.getBoolean("g.UseLineNumbers"));
    gd.setOutputEncoding(ps.getString("g.OutputEncoding"));
    gd.setTargetDirectory(ps.getString("g.TargetDirectory"));
    gd.setLinkSource(ps.getString("g.LinkSource"));

    gd.run(new GeneratorListener());
  }
  
  class GeneratorListener implements ActionListener{
    public void action(GeneratorDialog gd, int action) {
      switch(action){
        case GeneratorDialog.CLOSE_ACTION:
          IPreferenceStore ps = EclipsecolorerPlugin.getDefault().getPreferenceStore();
          ps.setValue("g.Prefix", gd.getPrefix());
          ps.setValue("g.Suffix", gd.getSuffix());
          ps.setValue("g.HRDSchema", gd.getHRDSchema());
          ps.setValue("g.HtmlHeaderFooter", gd.isHtmlHeaderFooter());
          ps.setValue("g.InfoHeader", gd.isInfoHeader());
          ps.setValue("g.UseLineNumbers", gd.isUseLineNumbers());
          ps.setValue("g.OutputEncoding", gd.getOutputEncoding());
          ps.setValue("g.TargetDirectory", gd.getTargetDirectory());
          if (gd.getLinkSource() != null){
            ps.setValue("g.LinkSource", gd.getLinkSource());
          }
          gd.getShell().close();
          break;
          
        case GeneratorDialog.GENERATE_ACTION:
          String[] fileList = gd.getFileList();
          String lastFileName = null;
          StringBuffer fileNames = new StringBuffer();
          String filePath = gd.getTargetDirectory(); 
          int num = 0;
          try{
            for(int i = 0; i < fileList.length; i++) {
              gd.setProgress((i+1)*100/fileList.length);
              String fileLocation = fileList[i];
              File file = new File(fileLocation);
              lastFileName = file.getName(); 
              fileNames.append(lastFileName).append("\n");
        
              ReaderLineSource rls = new ReaderLineSource(new FileReader(file));
              final String targetName = filePath + "/" + gd.getPrefix() + file.getName() + gd.getSuffix();
              Writer commonWriter = null;
              if ("default".equals(gd.getOutputEncoding())){
                commonWriter = new OutputStreamWriter(new FileOutputStream(targetName));
              }else{
                commonWriter = new OutputStreamWriter(new FileOutputStream(targetName), gd.getOutputEncoding());
              }
              Writer escapedWriter = null;
              if (gd.isHtmlSubst()){
                escapedWriter = new EscapedWriter(commonWriter);
              }else{
                escapedWriter = commonWriter;
              }
              
              ParserFactory pf = EclipsecolorerPlugin.getDefault().getParserFactory();
              HTMLGenerator hg = new HTMLGenerator(pf,rls,gd.getHRDSchema());

              hg.generate(commonWriter, escapedWriter,
                  file.getName(), gd.isUseLineNumbers(),
                  gd.isHtmlSubst(), gd.isInfoHeader(),
                  gd.isHtmlHeaderFooter());
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

          gd.setProgress(0);
          break;
      }
    }
  }
}