package net.sf.colorer.eclipse.outline;

import net.sf.colorer.Region;
import net.sf.colorer.editor.OutlineItem;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.ui.model.IWorkbenchAdapter;

public class OutlineElement extends OutlineItem implements IAdaptable{
  
  IWorkbenchAdapter ad = null;
  
  public OutlineElement(IWorkbenchAdapter ad, int lno, int pos, int level, String token, Region region) {
    super(lno, pos, level, token, region);	
    this.ad = ad;
  }
  
  public Object getAdapter(Class adapter) {
  	if (adapter == IWorkbenchAdapter.class) return ad;
  //	if (adapter == IPropertySource.class)
  //		return new MarkElementProperties(this);
  	return null;
  }

}
