package net.sf.colorer.eclipse.outline;

import java.io.File;
import java.util.Hashtable;
import java.util.Vector;

import net.sf.colorer.Region;
import net.sf.colorer.eclipse.ImageStore;
import net.sf.colorer.editor.OutlineItem;
import net.sf.colorer.editor.Outliner;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.model.IWorkbenchAdapter;

public class WorkbenchOutliner extends Outliner implements IWorkbenchAdapter, IAdaptable{

  Hashtable iconsHash = new Hashtable();
  Vector listeners = new Vector();
  boolean changed = true;
  boolean hierarchy = true;
  
  public WorkbenchOutliner(Region filter){
    super(filter);
  }
  
  public void addListener(OutlineListener listener){
    listeners.addElement(listener);
  }
  public void removeListener(OutlineListener listener){
    listeners.removeElement(listener);
  }
  public void setHierarchy(boolean hierarchy){
    this.hierarchy = hierarchy;
  }

  public OutlineItem createItem(int lno, int sx, int curLevel, String itemLabel, Region region) {
    return new OutlineElement(this, lno, sx, curLevel, itemLabel, region); 
  }
  public void startParsing(int lno){
    super.startParsing(lno);
    changed = true;
  }
  public void endParsing(int lno){
    super.endParsing(lno);
    if (changed)
      for(int idx = 0; idx < listeners.size(); idx++)
        ((OutlineListener)listeners.elementAt(idx)).notifyUpdate();
    changed = false;    
  };
  
  
  // represents root of Outline structure
  public Object getAdapter(Class adapter){
    if (adapter == IWorkbenchAdapter.class) return this;
    return null;
  }

  public Object[] getChildren(Object object) {
    Vector elements = new Vector();
    
    if (!hierarchy){
      if (object == this){
        for(int idx = 0; idx < itemCount(); idx++)
          elements.addElement(getItem(idx));
      };
    }else{
      if (object == this){
        int flevel = 0x100000;
        for(int idx = 0; idx < itemCount(); idx++){
          if (flevel > getItem(idx).level) flevel = getItem(idx).level;
//System.out.println(flevel+" - "+ getItem(idx).level + " - "+getItem(idx).token);
          if (getItem(idx).level > flevel) continue;
          elements.addElement(getItem(idx));
        }
      }else if (object instanceof OutlineElement){
        OutlineElement el = (OutlineElement)object;
        int idx = outline.indexOf(el);
        if (idx > -1){ 
          int flevel = 0x100000;
          for(idx++; idx < itemCount(); idx++){
            if (getItem(idx).level <= el.level) break;
            if (flevel > getItem(idx).level) flevel = getItem(idx).level; 
            if (getItem(idx).level > flevel) continue;
            elements.addElement(getItem(idx));
          }
        }
      }
    }
    return elements.toArray();
  }
  
  public ImageDescriptor getImageDescriptor(Object object) {
    String iconName = null;
    if (object instanceof OutlineElement){
      OutlineElement el = (OutlineElement)object;
      iconName = searchIcons(el.region);
    }
    if (iconName == null) return null;
  	return ImageStore.getID(iconName);
  }
  
  String searchIcons(Region region){
    if (region == null) return null;
    if (iconsHash.get(region.getName()) != null) return (String)iconsHash.get(region.getName());
    for(;region != null; region = region.getParent()){
      String iconName = "outline" + File.separator+region.getName().replace(':', File.separatorChar);
      ImageDescriptor id = ImageStore.getID(iconName);
      if (id != null){
        iconsHash.put(region.getName(), iconName);
        return iconName;
      }
    }
    return "outline"+File.separator+"def"+File.separator+"Outlined"; 
  }
  
  public String getLabel(Object object) {
    if (object instanceof OutlineElement){
      OutlineElement el = (OutlineElement)object;
      return el.token.toString();
    }
    return null;
  }
  
  public Object getParent(Object o) {
  	return null;
  }

}
