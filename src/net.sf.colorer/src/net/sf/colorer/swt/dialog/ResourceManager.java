package net.sf.colorer.swt.dialog;

import java.util.HashMap;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.widgets.Display;


public class ResourceManager{
  static HashMap m_FontMap = new HashMap();
  public static Font getFont(String name, int height, int style) {
    String fullName = name + "|" + height + "|" + style;
    Font font = (Font) m_FontMap.get(fullName);
    if (font == null) {
      font = new Font(Display.getCurrent(), name, height, style);
      m_FontMap.put(fullName, font);
    }
    return font;
  }
  public static Color getColor(int clr){
    return Display.getDefault().getSystemColor(clr);
  }
}
