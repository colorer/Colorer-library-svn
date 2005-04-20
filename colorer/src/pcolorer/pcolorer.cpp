
#include<windows.h>

#include<farplugin/plugin.hpp>

#include"pcolorer.h"
#include"FarEditorSet.h"

PluginStartupInfo Info;
FarEditorSet *editorSet = null;

/**
 * Returns message from FAR current language.
 */
const char *GetMsg(int msg){
  return(Info.GetMsg(Info.ModuleNumber, msg));
};

/**
 * Plugin initialization and creation of editor set support class.
 */
void WINAPI SetStartupInfo(const struct PluginStartupInfo *fei)
{
  memset(&Info, 0, sizeof(Info));
  memmove(&Info, fei, (fei->StructSize > sizeof(Info)) ? sizeof(Info) : fei->StructSize);

  editorSet = new FarEditorSet(&Info);
};

/**
 * Plugin strings in FAR interface.
 */
void WINAPI GetPluginInfo(struct PluginInfo *Info)
{
static char* PluginMenuStrings;
  memset(Info, 0, sizeof(*Info));
  Info->Flags = PF_EDITOR | PF_DISABLEPANELS;
  Info->StructSize = sizeof(*Info);
  Info->PluginConfigStringsNumber = 1;
  Info->PluginMenuStringsNumber = 1;
  PluginMenuStrings = (char*)GetMsg(mName);
  Info->PluginConfigStrings = &PluginMenuStrings;
  Info->PluginMenuStrings = &PluginMenuStrings;
  Info->CommandPrefix = "clr";
};

/**
 * On FAR exit. Destroys all internal structures.
 */
void WINAPI ExitFAR()
{
  delete editorSet;
};

/**
 * Open plugin configuration of actions dialog.
 */
HANDLE WINAPI OpenPlugin(int OpenFrom, int Item)
{
  if (OpenFrom == OPEN_EDITOR){
    editorSet->openMenu();
  }else if (OpenFrom == OPEN_COMMANDLINE){
    char *path = (char*)Item;
    editorSet->viewFile(DString(path));
  }else
    editorSet->configure();
  return INVALID_HANDLE_VALUE;
};

/**
 * Configures plugin.
 */
int WINAPI Configure(int ItemNumber)
{
  editorSet->configure();
  return 1;
};

/**
 * Processes FAR editor events and
 * makes text colorizing here.
 */
int WINAPI ProcessEditorEvent(int Event, void *Param)
{
  return editorSet->editorEvent(Event, Param);
};
int WINAPI ProcessEditorInput(const INPUT_RECORD*ir)
{
  return editorSet->editorInput(ir);
}

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