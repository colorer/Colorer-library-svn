package net.sf.colorer.editor;

import net.sf.colorer.Region;

public class OutlineItem{
  /** Line number */
  public int lno;
  /** Position in line */
  public int pos;
  /** Level of enclosure */
  public int level;
  /** Item text */
  public StringBuffer token;
  /** This item's region */
  public Region region;

  /** Default constructor */
  public OutlineItem(){
    lno = pos = 0;
    token = null;
  };
  /** Initializing constructor */
  public OutlineItem(int lno, int pos, int level, String token, Region region){
    this.lno = lno;
    this.pos = pos;
    this.level = level;
    this.region = region;
    this.token = null;
    if (token != null) this.token = new StringBuffer(token);
  };
  
  public String toString(){
    return ""+token+"("+level+")"; 
  }
  
}; 

