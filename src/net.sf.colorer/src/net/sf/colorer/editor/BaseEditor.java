package net.sf.colorer.editor;

import net.sf.colorer.*;
import net.sf.colorer.handlers.*;

public interface BaseEditor{

  /** LineRegionsSupport object preferences.
      Installs specified RegionStore (basically HRDRegionStore), which
      maps HRC Regions into color data, sets default size (in lines) of
      Regions structure cache.
      @param compact Creates LineRegionsSupport (false) or
      LineRegionsCompactSupport (true) object to store lists of RegionDefine's
  */
  public void setRegionCompact(boolean compact);

  /** Changes used file type */ 
  public void setFileType(String typename);
  /** Chooses filetype according to the filename and first line of text */ 
  public String chooseFileType(String fname);
  /** Returns Currently selected file type */ 
  public String getFileType();

  /** Installs specified RegionMapper. */ 
  public void setRegionMapper(String cls, String name);

  /** Adds specified RegionHandler object into the parse process.
   * @param filter If not null, handler would be activated only if
   * passed regions have specified <code>filter</code> parent.
   * This allows to optimize performance and disable unnecesary JNI
   * context switches.
  */
  void addRegionHandler(RegionHandler rh, Region filter);
  /** Removes previously added region handler.
  */
  void removeRegionHandler(RegionHandler rh);

  /** Current Background Region (def:Text) */ 
  public RegionDefine getBackground();
  /** Current Vertical Rule (def:VertCross) */ 
  public RegionDefine getVertCross();
  /** Current Horizontal Rule (def:HorzCross) */ 
  public RegionDefine getHorzCross();

  /** Searches and creates pair match object.
      Returned object can lately be used in pair search methods.
      @param lineNo Line number, where to search paired region.
      @param pos Position in line, where paired region to be searched.
             Paired Region is found, if it includes
             specified position or ends directly
             at one char before line position.
  */
  public PairMatch getPairMatch(int lineNo, int pos);

  /** Searches pair match in currently avaiable parsed information.
      @param pm Unmatched pair match
  */
  public void searchLocalPair(PairMatch pm);
  /** Searches pair match in all avaiable text.
      @param pm Unmatched pair match
  */
  public void searchGlobalPair(PairMatch pm);

  /** Return parsed and colored LineRegions of requested line.
      This method validates current cache state
      and, if needed, calls Colorer parser to
      validate modified block of text.
      Size of reparsed text is choosed according to information
      about visible text range and modification events.
  */
  public LineRegion[] getLineRegions(int lno);
  /** Informs BaseEditor object about text modification event.
      All text after specified line becomes invalid.
      @param topLine Topmost modified line of text.
  */
  public void modifyEvent(int topLine);

  /** Informs about single line modification event.
      Generally, this type of event can be processed much faster
      because of pre-checking line's changed structure and
      cancelling further parsing in case of unmodified text structure.
      @todo Not used yet!
      @param line Modified line of text.
  */
  public void modifyLineEvent(int line);

  /** Informs about changes in visible range of text lines.
      This information is used to make assumptions about
      text structure and to make faster parsing.
      @param wStart Topmost visible line of text.
      @param wSize  Number of currently visible text lines.
  */
  public void visibleTextEvent(int wStart, int wSize);

  /** Informs about total lines count change. */
  public void lineCountEvent(int newLineCount);

};
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