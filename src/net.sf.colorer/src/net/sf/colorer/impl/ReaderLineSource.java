package net.sf.colorer.impl;

import java.io.IOException;
import java.io.LineNumberReader;
import java.io.Reader;
import java.util.Vector;

import net.sf.colorer.LineSource;

/**
 * Implements LineSource interface, reading
 * data from passed Reader stream. 
 */
public class ReaderLineSource implements LineSource {

  LineNumberReader reader;
  Vector lines = new Vector();
  int lineCount = 1;
  
  public ReaderLineSource(Reader reader){
    this.reader = new LineNumberReader(reader);
  }
  
  /** 
   * Returns total line count in passed reader
   */
  public int getLineCount(){
    while (getLine(lineCount) != null);
    return lineCount;
  }
  
  public String getLine(int lno) {
    while (lines.size() <= lno){
      String line = null;
      try{
        line = reader.readLine();
      }catch(IOException e){ return null; }
      if (line == null) break;
      lines.addElement(line);
      lineCount = lines.size();
    }
    if (lines.size() <= lno) return null;
    return (String)lines.elementAt(lno);
  }

}
