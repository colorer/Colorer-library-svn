
import net.sf.colorer.*;
import net.sf.colorer.editor.*;
import net.sf.colorer.impl.*;

import java.util.*;

class Test{

public static void main(String[] arr){

  ParserFactory pf = new ParserFactory();

  for(Enumeration e = pf.enumerateHRDInstances("rgb"); e.hasMoreElements();){
    String name = (String)e.nextElement();
    System.out.println(name + " - " + pf.getHRDescription("rgb", name));
  };

  BaseEditor be = new BaseEditorNative(pf, new LineSource(){
    public String getLine(int no){ return null; };
  });

  for(Enumeration e = pf.enumerateFileTypes(); e.hasMoreElements();){
    String type = (String)e.nextElement();
    System.out.println("loading: "+type);
    be.setFileType(type);
  };

};

}