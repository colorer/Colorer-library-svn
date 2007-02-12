#ifndef _FAREDITORSET_H_
#define _FAREDITORSET_H_

#include<colorer/handlers/FileErrorHandler.h>
#include<colorer/handlers/LineRegionsSupport.h>
#include<colorer/handlers/StyledHRDMapper.h>

#include"pcolorer.h"

#include"FarEditor.h"

/**
 * FAR Editors container.
 * Manages all library resources and creates FarEditor class instances.
 * @ingroup far_plugin
 */
class FarEditorSet{
public:
  /** Creates set and initialises it with PluginStartupInfo structure */
  FarEditorSet(PluginStartupInfo *fedi);
  /** Standard destructor */
  ~FarEditorSet();

  /** Shows editor actions menu */
  void openMenu();
  /** Shows plugin's configuration dialog */
  void configure();
  /** Views current file with internal viewer */
  void viewFile(const String &path);

  /** Dispatch editor event in the opened editor */
  int  editorEvent(int Event, void *Param);
  /** Dispatch editor input event in the opened editor */
  int  editorInput(const INPUT_RECORD *ir);

private:
  /** Returns current global error handler. */
  ErrorHandler *getErrorHandler();
  /** Returns currently active editor. */
  FarEditor *getCurrentEditor();
  /**
   * Reloads HRC database.
   * Drops all currently opened editors and their
   * internal structures. Prepares to work with newly
   * loaded database.
   */
  void reloadBase();

  /** Shows dialog of file type selection */
  void chooseType();
  /** Shows dialog with HRD scheme selection */
  const String *chooseHRDName(const String *current);
  /** FAR localized messages */
  const char *GetMsg(int msg);
  /** Reads all registry settings into variables */
  void readRegistry();
  /** Kills all currently opened editors */
  void dropAllEditors();
  /** Disables all plugin processing */
  void disableColorer();

  Hashtable<FarEditor*> farEditorInstances;
  ParserFactory *parserFactory;
  RegionMapper *regionMapper;
  HRCParser *hrcParser;

  PluginStartupInfo *info;
  HKEY hPluginRegistry;

  /** registry settings */
  bool rDisabled;
  bool drawCross, drawPairs, drawSyntax;
  bool oldOutline;
  int  rMaxTime;
};

#endif
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
 * Cail Lomecb <cail@nm.ru>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2005
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